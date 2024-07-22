#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>
#include <threads.h>
#include <math.h>
#include "volume.h"
#include "raw.h"
#include "matrix.h"
#include "octree.h"

#define container_of(ptr, type, member) (type *)((char *)(ptr) - (char *) &((type *)0)->member)

typedef int (*PFN_volume_open)(FILE*, struct volume*, const void*);


static PFN_volume_open volume_open_table[] =
{
	raw_open,
	NULL,
};


bool is_volume_valid(const struct volume* volume)
{
	if(
		volume->params.width < 0 ||
		volume->params.height < 0 ||
		volume->params.depth < 0 ||
		volume->params.widthscale <= 0.0 ||
		volume->params.heightscale <= 0.0 ||
		volume->params.depthscale <= 0.0 ||
		volume->params.voxelformat < 0 ||
		volume->params.voxelformat >= NUMBER_OF_VOXEL_FORMAT
	) return false;

	return true;
}

struct volume_arg
{
	struct volume*	volume;

	thrd_t	thread;
	size_t	begin;
	size_t	end;
};

static void set_range(struct volume* volume, struct volume_arg* arg, const int nth, const int total)
{
	arg->volume = volume;
	arg->begin = volume->nr_voxels / total * nth;
	arg->end = volume->nr_voxels / total * (nth + 1) - 1;

	if(arg->end >= volume->nr_voxels)
		arg->end = volume->nr_voxels - 1;
}

static int convert_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct volume* volume = volume_arg->volume;
	int bitmask = (1 << volume->params.bitmask) - 1;

	switch (volume->params.voxelformat)
	{
	case VOXEL_FORMAT_UNSIGNED_8:
		for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
			volume->data.u8[i] &= bitmask;
		break;
	case VOXEL_FORMAT_UNSIGNED_16_LE:
		for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
			volume->data.u16[i] &= bitmask;
		break;
	case VOXEL_FORMAT_UNSIGNED_16_BE:
		for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
			volume->data.u16[i] = (bswap_16(volume->data.u16[i]) & bitmask);
		break;
	default:
		break;
	}

	return 0;
}

static int volume_convert_voxels(struct volume* volume)
{
	if(volume->params.voxelformat != VOXEL_FORMAT_UNSIGNED_16_BE && volume->params.bitmask == 16)
		return 0;

	int nr_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	struct volume_arg* volume_arg = calloc(1, sizeof(struct volume_arg) * nr_cpu);

	for(int i = 0;i < nr_cpu;++i)
	{
		set_range(volume, &volume_arg[i], i, nr_cpu);
		thrd_create(&volume_arg[i].thread, convert_thread, &volume_arg[i]);
	}

	for(int i = 0;i < nr_cpu;++i)
		thrd_join(volume_arg[i].thread, NULL);

	free(volume_arg);

	if(volume->params.voxelformat == VOXEL_FORMAT_UNSIGNED_16_BE)
		volume->params.voxelformat = VOXEL_FORMAT_UNSIGNED_16_LE;

	return 0;
}

#define get_index(v, x, y, z)	(v->params.width * v->params.height * (z) + v->params.width * (y) + (x))


struct gradient_arg
{
	float max_length;
	struct volume_arg volume_arg;
};


static int calculate_gradient_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct gradient_arg* gradient_arg = container_of(volume_arg, struct gradient_arg, volume_arg);
	struct volume* volume = volume_arg->volume;

	float max_length = 0.0f;

	for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
	{
		int x, y, z;

		z = i / (volume->params.width * volume->params.height);
		y = (i / volume->params.width) % volume->params.height;
		x = i % volume->params.width;

		int ax, ay, az;
		int bx, by, bz;

		ax = (x == 0) ? 0 : x - 1;
		bx = (x == (volume->params.width - 1)) ? (volume->params.width - 1) : x + 1;
		ay = (y == 0) ? 0 : y - 1;
		by = (y == (volume->params.height - 1)) ? (volume->params.height - 1) : y + 1;
		az = (z == 0) ? 0 : z - 1;
		bz = (z == (volume->params.depth - 1)) ? (volume->params.depth - 1) : z + 1;

		float gradient[4];

		switch(volume->params.voxelformat)
		{
		case VOXEL_FORMAT_UNSIGNED_8:
			gradient[0] = (volume->data.u8[get_index(volume, bx, y, z)] - volume->data.u8[get_index(volume, ax, y, z)]) / 2.0;
			gradient[1] = (volume->data.u8[get_index(volume, x, by, z)] - volume->data.u8[get_index(volume, x, ay, z)]) / 2.0;
			gradient[2] = (volume->data.u8[get_index(volume, x, y, bz)] - volume->data.u8[get_index(volume, x, y, az)]) / 2.0;
			break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
			gradient[0] = (volume->data.u16[get_index(volume, bx, y, z)] - volume->data.u16[get_index(volume, ax, y, z)]) / 2.0;
			gradient[1] = (volume->data.u16[get_index(volume, x, by, z)] - volume->data.u16[get_index(volume, x, ay, z)]) / 2.0;
			gradient[2] = (volume->data.u16[get_index(volume, x, y, bz)] - volume->data.u16[get_index(volume, x, y, az)]) / 2.0;
			break;
		default:
			break;
		}

		gradient[3] = vec_length(gradient, 3);

		if(gradient[3] != 0.0f)
			vec_normalize(gradient, 3);

		volume->gradient[i * 4 + 0] = gradient[0];
		volume->gradient[i * 4 + 1] = gradient[1];
		volume->gradient[i * 4 + 2] = gradient[2];
		volume->gradient[i * 4 + 3] = gradient[3];

		if(max_length < gradient[3])
			max_length = gradient[3];
	}

	gradient_arg->max_length = max_length;

	return 0;
}

static int normalize_gradient_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct volume* volume = volume_arg->volume;

	for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
		volume->gradient[i * 4 + 3] /= volume->max_gradient_length;

	return 0;
}

static int volume_calculate_gradient(struct volume* volume)
{
	if(volume->gradient)
		return 0;

	volume->gradient = calloc(1, volume->nr_voxels * sizeof(float) * 4);

	int nr_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	struct gradient_arg* gradient_arg = calloc(1, sizeof(struct gradient_arg) * nr_cpu);
	float max_length = 0.0f;

	for(int i = 0;i < nr_cpu;++i)
	{
		set_range(volume, &gradient_arg[i].volume_arg, i, nr_cpu);
		thrd_create(&gradient_arg[i].volume_arg.thread, calculate_gradient_thread, &gradient_arg[i].volume_arg);
	}

	for(int i = 0;i < nr_cpu;++i)
	{
		thrd_join(gradient_arg[i].volume_arg.thread, NULL);
		if(max_length < gradient_arg[i].max_length)
			max_length = gradient_arg[i].max_length;
	}

	volume->max_gradient_length = max_length;

	for(int i = 0;i < nr_cpu;++i)
	{
		set_range(volume, &gradient_arg[i].volume_arg, i, nr_cpu);
		thrd_create(&gradient_arg[i].volume_arg.thread, normalize_gradient_thread, &gradient_arg[i].volume_arg);
	}

	for(int i = 0;i < nr_cpu;++i)
		thrd_join(gradient_arg[i].volume_arg.thread, NULL);

	free(gradient_arg);

	return 0;
}

struct volume* volume_open(const char* filename, const enum volume_file_type type, const void* parameters)
{
	struct volume* volume = NULL;
	FILE* fp = NULL;

	if(type < 0 || type >= NUMBER_OF_VOLUME_FILE_TYPE)
		return NULL;

	if(access(filename, F_OK))
		return NULL;

	fp = fopen(filename, "r");

	if(!fp)
		return NULL;

	volume = calloc(1, sizeof(struct volume));

	if(volume_open_table[type](fp, volume, parameters))
	{
		if(volume->data.u8) free(volume->data.u8);
		free(volume);
		volume = NULL;
	}

	fclose(fp);

	if(!volume)
		return NULL;

	volume_convert_voxels(volume);

	volume->octree_node = octree_create(volume);
	volume->maxvalue = volume->octree_node[0].maximum;

	volume_calculate_gradient(volume);

	return volume;
}


int volume_destroy(struct volume* volume)
{
	if(!volume)
		return 1;

	if(volume->data.u8)
		free(volume->data.u8);

	if(volume->octree_node)
		free(volume->octree_node);

	if(volume->gradient)
		free(volume->gradient);

	free(volume);

	return 0;
}

int volume_write_to_file(const struct volume* volume, const char* filename)
{
	if(!is_volume_valid(volume))
		return 1;

	FILE* fp = fopen(filename, "w");

	if(!fp)
		return 1;

	fwrite(volume->data.u8, 1, volume->datasize, fp);
	fclose(fp);

	return 0;
}

int sizeof_voxel_format(const enum voxel_format format)
{
	int size = 0;

	switch(format)
	{
	case VOXEL_FORMAT_UNSIGNED_8:
		size = 1;
		break;
	case VOXEL_FORMAT_UNSIGNED_16_LE:
	case VOXEL_FORMAT_UNSIGNED_16_BE:
		size = 2;
		break;
	default:
		size = 0;
		break;
	}

	return size;
}

struct histogram_arg
{
	uint64_t*	histogram;
	int	size;
	struct volume_arg volume_arg;
};

static int gen_histogram_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct histogram_arg* histogram_arg = container_of(volume_arg, struct histogram_arg, volume_arg);
	struct volume* volume = volume_arg->volume;

	for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
	{
		switch(volume->params.voxelformat)
		{
		case VOXEL_FORMAT_UNSIGNED_8:
			++histogram_arg->histogram[volume->data.u8[i] * histogram_arg->size / (volume->maxvalue + 1)];
			break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
			++histogram_arg->histogram[volume->data.u16[i] * histogram_arg->size / (volume->maxvalue + 1)];
			break;
		default:
			break;
		}
	}

	return 0;
}

void volume_gen_histogram(struct volume* volume, float* histogram, uint64_t* maximum, const size_t size)
{
	int nr_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	struct histogram_arg* histogram_arg = calloc(1, sizeof(struct histogram_arg) * nr_cpu);
	uint64_t max = 0;
	uint64_t max_raw = 0;
	uint64_t* histogram_raw  = calloc(1, sizeof(uint64_t) * size);

	for(int i = 0;i < nr_cpu;++i)
	{
		histogram_arg[i].size = size;
		histogram_arg[i].histogram = calloc(1, sizeof(uint64_t) * size);
		set_range(volume, &histogram_arg[i].volume_arg, i, nr_cpu);
		thrd_create(&histogram_arg[i].volume_arg.thread, gen_histogram_thread, &histogram_arg[i].volume_arg);
	}

	for(int i = 0;i < nr_cpu;++i)
	{
		thrd_join(histogram_arg[i].volume_arg.thread, NULL);

		for(int j = 0;j < size;++j)
			histogram_raw[j] += histogram_arg[i].histogram[j];

		free(histogram_arg[i].histogram);
	}

	for(int i = 0;i < size;++i)
	{
		if(max_raw < histogram_raw[i])
			max_raw = histogram_raw[i];

		histogram_raw[i] = histogram_raw[i] ? log(histogram_raw[i]) * 100 : 0;

		if(max < histogram_raw[i])
			max = histogram_raw[i];
	}

	for(int i = 0;i < size;++i)
		histogram[i] = (float)histogram_raw[i] / max;

	free(histogram_raw);
	free(histogram_arg);

	if(maximum)
		*maximum = max_raw;
}

struct histogram2d_arg
{
	uint64_t*	histogram;

	int	sizeof_value;
	int	size_magnitude;

	struct volume_arg volume_arg;
};

static int gen_histogram2d_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct histogram2d_arg* histogram2d_arg = container_of(volume_arg, struct histogram2d_arg, volume_arg);
	struct volume* volume = volume_arg->volume;

	for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
	{
		int value = 0;
		int magnitude = volume->gradient[i * 4 + 3] * (histogram2d_arg->size_magnitude - 1);

		switch(volume->params.voxelformat)
		{
		case VOXEL_FORMAT_UNSIGNED_8:
			value = volume->data.u8[i] * histogram2d_arg->sizeof_value / (volume->maxvalue + 1);
			break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
			value = volume->data.u16[i] * histogram2d_arg->sizeof_value / (volume->maxvalue + 1);
			break;
		default:
			break;
		}

		++histogram2d_arg->histogram[magnitude * histogram2d_arg->sizeof_value + value];
	}

	return 0;
}

void volume_gen_2d_histogram(struct volume* volume, float* histogram, uint64_t* maximum, const size_t sizeof_value, const size_t size_magnitude)
{
	int nr_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	struct histogram2d_arg* histogram2d_arg = calloc(1, sizeof(struct histogram2d_arg) * nr_cpu);
	uint64_t max = 0;
	uint64_t max_raw = 0;
	uint64_t* histogram_raw  = calloc(1, sizeof(uint64_t) * sizeof_value * size_magnitude);


	for(int i = 0;i < nr_cpu;++i)
	{
		histogram2d_arg[i].sizeof_value = sizeof_value;
		histogram2d_arg[i].size_magnitude = size_magnitude;
		histogram2d_arg[i].histogram = calloc(1, sizeof(uint64_t) * sizeof_value * size_magnitude);
		set_range(volume, &histogram2d_arg[i].volume_arg, i, nr_cpu);
		thrd_create(&histogram2d_arg[i].volume_arg.thread, gen_histogram2d_thread, &histogram2d_arg[i].volume_arg);
	}

	for(int i = 0;i < nr_cpu;++i)
	{
		thrd_join(histogram2d_arg[i].volume_arg.thread, NULL);

		for(int j = 0;j < sizeof_value * size_magnitude;++j)
			histogram_raw[j] += histogram2d_arg[i].histogram[j];

		free(histogram2d_arg[i].histogram);
	}

	for(int i = 0;i < sizeof_value * size_magnitude;++i)
	{
		if(max_raw < histogram_raw[i])
			max_raw = histogram_raw[i];

		histogram_raw[i] = histogram_raw[i] ? log(histogram_raw[i]) * 100 : 0;

		if(max < histogram_raw[i])
			max = histogram_raw[i];

	}

	for(int i = 0;i < sizeof_value * size_magnitude;++i)
		histogram[i] = (float)histogram_raw[i] / max;

	free(histogram_raw);
	free(histogram2d_arg);

	if(maximum)
		*maximum = max_raw;
}

struct downscale_arg
{
	const struct volume*	input;

	struct volume_arg volume_arg;
};


static int downscale_thread(void* arg)
{
	struct volume_arg* volume_arg = (struct volume_arg*)arg;
	struct downscale_arg* downscale_arg = container_of(volume_arg, struct downscale_arg, volume_arg);
	struct volume* volume = volume_arg->volume;
	const struct volume* input = downscale_arg->input;


	for(size_t i = volume_arg->begin;i <= volume_arg->end;++i)
	{
		int value = 0;
		int count = 0;
		int x, y, z;

		z = 2 * (i / (volume->params.width * volume->params.height));
		y = 2 * ((i / volume->params.width) % volume->params.height);
		x = 2 * (i % volume->params.width);

		for(int j = 0;j < 8;++j)
		{
			int dx, dy, dz;

			dx = j & 0x1;
			dy = j & 0x2;
			dz = j & 0x4;

			if((x + dx) >= input->params.width || (y + dy) >= input->params.height || (z + dz) >= input->params.depth)
				continue;

			switch(volume->params.voxelformat)
			{
			case VOXEL_FORMAT_UNSIGNED_8:
				value += input->data.u8[get_index(input, x + dx, y + dy, z + dz)];
				break;
			case VOXEL_FORMAT_UNSIGNED_16_LE:
				value += input->data.u16[get_index(input, x + dx, y + dy, z + dz)];
				break;
			default:
				break;
			}

			++count;
		}

		value /= count;

		switch(volume->params.voxelformat)
		{
		case VOXEL_FORMAT_UNSIGNED_8:
			volume->data.u8[i] = value;
			break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
			volume->data.u16[i] = value;
			break;
		default:
			break;
		}
	}

	return 0;
}


struct volume*	volume_create_downscaled(const struct volume* volume)
{
	int nr_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	struct downscale_arg* downscale_arg = calloc(1, sizeof(struct downscale_arg) * nr_cpu);

	struct volume* new = NULL;

	new = calloc(1, sizeof(struct volume));

	new->params.width = volume->params.width / 2;
	new->params.height = volume->params.height / 2;
	new->params.depth = volume->params.depth / 2;
	new->nr_voxels = new->params.width * new->params.height * new->params.depth;

	new->params.widthscale = volume->params.widthscale;
	new->params.heightscale = volume->params.heightscale;
	new->params.depthscale = volume->params.depthscale;

	new->params.voxelformat = volume->params.voxelformat;
	new->datasize = new->nr_voxels * sizeof_voxel_format(new->params.voxelformat);

	new->data.u8 = calloc(1, volume->datasize);

	for(int i = 0;i < nr_cpu;++i)
	{
		set_range(new, &downscale_arg[i].volume_arg, i, nr_cpu);
		downscale_arg[i].input = volume;
		thrd_create(&downscale_arg[i].volume_arg.thread, downscale_thread, &downscale_arg[i].volume_arg);
	}

	for(int i = 0;i < nr_cpu;++i)
		thrd_join(downscale_arg[i].volume_arg.thread, NULL);

	free(downscale_arg);

	new->octree_node = octree_create(new);
	new->maxvalue = new->octree_node[0].maximum;
	volume_calculate_gradient(new);

	return new;
}

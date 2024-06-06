#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>
#include <math.h>
#include "volume.h"
#include "raw.h"
#include "matrix.h"

typedef int (*PFN_volume_open)(FILE*, struct volume*, const void*);


static PFN_volume_open volume_open_table[] =
{
	raw_open,
	NULL,
};


bool is_volume_valid(const struct volume* volume)
{
	if(
		volume->width < 0 ||
		volume->height < 0 ||
		volume->depth < 0 ||
		volume->widthscale <= 0.0 ||
		volume->heightscale <= 0.0 ||
		volume->depthscale <= 0.0 ||
		volume->voxelformat < 0 ||
		volume->voxelformat >= NUMBER_OF_VOXEL_FORMAT
	) return false;

	return true;
}

static int volume_fix_endian(struct volume* volume)
{
	if(!is_volume_valid(volume) || !volume->data.u8 || volume->voxelformat == VOXEL_FORMAT_UNSIGNED_8 || volume->voxelformat == VOXEL_FORMAT_UNSIGNED_16_LE)
		return 1;

	switch(volume->voxelformat)
	{
	case VOXEL_FORMAT_UNSIGNED_12_LE:
		for(int i = 0;i < volume->nr_voxels;++i)
			volume->data.u16[i] &= ((1 << 12) - 1);
		break;
	case VOXEL_FORMAT_UNSIGNED_12_BE:
		for(int i = 0;i < volume->nr_voxels;++i)
			volume->data.u16[i] = bswap_16(volume->data.u16[i]) & ((1 << 12) - 1);
		break;
	case VOXEL_FORMAT_UNSIGNED_16_BE:
		for(int i = 0;i < volume->nr_voxels;++i)
			volume->data.u16[i] = bswap_16(volume->data.u16[i]);
		break;
	default:
		break;
	}

	volume->voxelformat = VOXEL_FORMAT_UNSIGNED_16_LE;

	return 0;
}

static int find_max_u8(const uint8_t* array, const size_t length)
{
	int max = 0;

	for(int i = 0;i < length;++i)
		max = (array[i] > max) ? array[i] : max;

	return max;
}

static int find_max_u16(const uint16_t* array, const size_t length)
{
	int max = 0;

	for(int i = 0;i < length;++i)
		max = (array[i] > max) ? array[i] : max;

	return max;
}

#define get_index(x, y, z)	(volume->width * volume->height * (z) + volume->width * (y) + (x))

static void volume_calculate_gradient(struct volume* volume)
{
	float max_length = 0.0f;

	if(volume->gradient)
		return;

	volume->gradient = calloc(1, volume->nr_voxels * sizeof(float) * 4);

	for(int i = 0;i < volume->nr_voxels;++i)
	{
		int x, y, z;

		z = i / (volume->width * volume->height);
		y = (i / volume->width) % volume->height;
		x = i % volume->width;

		int ax, ay, az;
		int bx, by, bz;

		ax = (x == 0) ? 0 : x - 1;
		bx = (x == (volume->width - 1)) ? (volume->width - 1) : x + 1;
		ay = (y == 0) ? 0 : y - 1;
		by = (y == (volume->height - 1)) ? (volume->height - 1) : y + 1;
		az = (z == 0) ? 0 : z - 1;
		bz = (z == (volume->depth - 1)) ? (volume->depth - 1) : z + 1;

		float gradient[4];

		switch(volume->voxelformat)
		{
		case VOXEL_FORMAT_UNSIGNED_8:
			gradient[0] = (volume->data.u8[get_index(bx, y, z)] - volume->data.u8[get_index(ax, y, z)]) / 2.0;
			gradient[1] = (volume->data.u8[get_index(x, by, z)] - volume->data.u8[get_index(x, ay, z)]) / 2.0;
			gradient[2] = (volume->data.u8[get_index(x, y, bz)] - volume->data.u8[get_index(x, y, az)]) / 2.0;
			break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
			gradient[0] = (volume->data.u16[get_index(bx, y, z)] - volume->data.u16[get_index(ax, y, z)]) / 2.0;
			gradient[1] = (volume->data.u16[get_index(x, by, z)] - volume->data.u16[get_index(x, ay, z)]) / 2.0;
			gradient[2] = (volume->data.u16[get_index(x, y, bz)] - volume->data.u16[get_index(x, y, az)]) / 2.0;
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

	for(int i = 0;i < volume->nr_voxels;++i)
		volume->gradient[i * 4 + 3] /= max_length;

	volume->max_gradient_length = max_length;
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

	volume_fix_endian(volume);

	switch(volume->voxelformat)
	{
		case VOXEL_FORMAT_UNSIGNED_8:
		volume->maxvalue = find_max_u8(volume->data.u8, volume->nr_voxels);
		break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
		volume->maxvalue = find_max_u16(volume->data.u16, volume->nr_voxels);
		break;
		default:
		break;
	}

	volume_calculate_gradient(volume);

	return volume;
}


int volume_destroy(struct volume* volume)
{
	if(!volume)
		return 1;

	if(volume->data.u8)
		free(volume->data.u8);

	if(volume->gradient)
		free(volume->gradient);

	free(volume);

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
	case VOXEL_FORMAT_UNSIGNED_12_LE:
	case VOXEL_FORMAT_UNSIGNED_12_BE:
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

void volume_gen_histogram(const struct volume* volume, float* histogram, uint64_t* maximum, const size_t size)
{
	uint64_t max = 0;
	uint64_t max_raw = 0;
	uint64_t* histogram_raw  = calloc(1, sizeof(uint64_t) * size);

	switch(volume->voxelformat)
	{
	case VOXEL_FORMAT_UNSIGNED_8:
		for(int i = 0;i < volume->nr_voxels;++i)
			++histogram_raw[volume->data.u8[i] * size / (volume->maxvalue + 1)];
		break;
	case VOXEL_FORMAT_UNSIGNED_16_LE:
		for(int i = 0;i < volume->nr_voxels;++i)
			++histogram_raw[volume->data.u16[i] * size / (volume->maxvalue + 1)];
	default:
		break;
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

	if(maximum)
		*maximum = max_raw;
}

void volume_gen_2d_histogram(const struct volume* volume, float* histogram, uint64_t* maximum, const size_t sizeof_value, const size_t size_magnitude)
{
	uint64_t max = 0;
	uint64_t max_raw = 0;
	uint64_t* histogram_raw  = calloc(1, sizeof(uint64_t) * sizeof_value * size_magnitude);

	switch(volume->voxelformat)
	{
	case VOXEL_FORMAT_UNSIGNED_8:
		for(int i = 0;i < volume->nr_voxels;++i)
		{
			int value = volume->data.u8[i] * sizeof_value / (volume->maxvalue + 1);
			int magnitude = volume->gradient[i * 4 + 3] * (size_magnitude - 1);
			++histogram_raw[magnitude * sizeof_value + value];
		}
		break;
	case VOXEL_FORMAT_UNSIGNED_16_LE:
		for(int i = 0;i < volume->nr_voxels;++i)
		{
			int value = volume->data.u16[i] * sizeof_value / (volume->maxvalue + 1);
			int magnitude = volume->gradient[i * 4 + 3] * (size_magnitude - 1);
			++histogram_raw[magnitude * sizeof_value + value];
		}
	default:
		break;
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

	if(maximum)
		*maximum = max_raw;
}

#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum volume_file_type
{
	VOLUME_FILE_TYPE_RAW,
	NUMBER_OF_VOLUME_FILE_TYPE,
};

enum voxel_format
{
	VOXEL_FORMAT_UNSIGNED_8,
	VOXEL_FORMAT_UNSIGNED_12_LE,
	VOXEL_FORMAT_UNSIGNED_12_BE,
	VOXEL_FORMAT_UNSIGNED_16_LE,
	VOXEL_FORMAT_UNSIGNED_16_BE,
	NUMBER_OF_VOXEL_FORMAT,
};

struct raw_params
{
	int	width;
	float	widthscale;
	int	height;
	float	heightscale;
	int	depth;
	float	depthscale;

	enum voxel_format	voxelformat;
};

struct volume
{
	union
	{
		uint8_t*	u8;
		uint16_t*	u16;
	}data;

	float*	gradient;

	int	maxvalue;
	float	max_gradient_length;

	int64_t	nr_voxels;
	int64_t datasize;

	int	width;
	float	widthscale;
	int	height;
	float	heightscale;
	int	depth;
	float	depthscale;

	enum voxel_format	voxelformat;
};

struct volume*	volume_open(const char* filename, const enum volume_file_type type, const void* parameters);
int		volume_destroy(struct volume* volume);
int		sizeof_voxel_format(const enum voxel_format format);
void volume_gen_histogram(struct volume* volume, float* histogram, uint64_t* maximum, const size_t size);
void volume_gen_2d_histogram(struct volume* volume, float* histogram, uint64_t* maximum, const size_t sizeof_value, const size_t size_magnitude);
bool is_volume_valid(const struct volume* volume);

#endif /* __VOLUME_H__ */

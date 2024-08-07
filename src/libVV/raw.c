#include <stdio.h>
#include <stdlib.h>
#include "volume.h"
#include "raw.h"


int raw_open(FILE* fp, struct volume* volume, const void* raw_params)
{
	const struct raw_params* params = raw_params;

	int64_t read_size = 0;

	volume->params.width	= params->width;
	volume->params.height	= params->height;
	volume->params.depth	= params->depth;

	volume->nr_voxels = params->width * params->height * params->depth;

	volume->params.widthscale	= params->widthscale <= 0.0 ? 1.0 : params->widthscale;
	volume->params.heightscale	= params->heightscale <= 0.0 ? 1.0 : params->heightscale;
	volume->params.depthscale	= params->depthscale <= 0.0 ? 1.0 : params->depthscale;

	volume->params.voxelformat = params->voxelformat;
	volume->params.bitmask = params->bitmask;

	if(!is_volume_valid(volume))
		return 1;

	volume->datasize = volume->nr_voxels * sizeof_voxel_format(volume->params.voxelformat);
	volume->data.u8 = malloc(volume->datasize);

	read_size = fread(volume->data.u8, 1, volume->datasize, fp);

	if(read_size != volume->datasize)
		return 1;

	return 0;
}

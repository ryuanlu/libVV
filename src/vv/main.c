#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "vv.h"
#include "volume_rendering.h"
#include "isosurface_extraction.h"

extern const char _binary_help_txt_start;
extern const char _binary_help_txt_end;
#define _binary_help_txt_size	(int)(&_binary_help_txt_end - &_binary_help_txt_start)

static int parse_options(int argc, char** argv, struct vv_options* options)
{
	int c;

	if(argc == 1)
	{
		fprintf(stderr, "%.*s", _binary_help_txt_size, &_binary_help_txt_start);
		return 1;
	}

	while(1)
	{
		c = getopt(argc, argv, "F:f:s:r:i:I:d");

		if(c == -1)
			break;

		switch(c)
		{
		case 'F':
			if(!strcmp("raw", optarg))
				options->type = VOLUME_FILE_TYPE_RAW;
			break;
		case 'f':
			if(!strcmp("u8", optarg))
				options->params.voxelformat = VOXEL_FORMAT_UNSIGNED_8;
			if(!strcmp("u12le", optarg))
				options->params.voxelformat = VOXEL_FORMAT_UNSIGNED_12_LE;
			if(!strcmp("u12be", optarg))
				options->params.voxelformat = VOXEL_FORMAT_UNSIGNED_12_BE;
			if(!strcmp("u16le", optarg))
				options->params.voxelformat = VOXEL_FORMAT_UNSIGNED_16_LE;
			if(!strcmp("u16be", optarg))
				options->params.voxelformat = VOXEL_FORMAT_UNSIGNED_16_BE;
			break;
		case 's':
			sscanf(optarg, "%dx%dx%d", &options->params.width, &options->params.height, &options->params.depth);
			break;
		case 'r':
			sscanf(optarg, "%f:%f:%f", &options->params.widthscale, &options->params.heightscale, &options->params.depthscale);
			break;
		case 'i':
			strncpy(options->filename, optarg, PATH_MAX - 1);
			break;
		case 'I':
			options->action = VV_ACTION_ISO_SURFACE_EXTRACTION;
			options->isovalue = atoi(optarg);
			break;
		case 'd':
			options->downscale = 1;
			break;
		default:
			return 1;
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	struct volume* volume = NULL;
	struct vv_options options;

	memset(&options, 0, sizeof(struct vv_options));

	if(parse_options(argc, argv, &options))
	{
		exit(EXIT_FAILURE);
	}

	volume = volume_open(options.filename, options.type, &options.params);

	if(options.downscale)
	{
		struct volume* origin = volume;
		volume = volume_create_downscaled(origin);
		volume_destroy(origin);
	}

	if(!volume)
	{
		fprintf(stderr, "error\n");
		exit(EXIT_FAILURE);
	}

	switch(options.action)
	{
	case VV_ACTION_VOLUME_RENDERING:
		run_volume_rendering(volume, &options);
		break;
	case VV_ACTION_ISO_SURFACE_EXTRACTION:
		run_iso_surface_extraction(volume, &options, options.isovalue);
		break;
	default:
		break;
	}

	volume_destroy(volume);

	return EXIT_SUCCESS;
}

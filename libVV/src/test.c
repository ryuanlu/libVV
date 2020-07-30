#include <stdio.h>
#include <stdlib.h>
#include "VV.h"

#define TEST_DATA_PATH	"../cthead.bin"
#define TEST_DATA_WIDTH		(256)
#define TEST_DATA_HEIGHT	(256)
#define TEST_DATA_DEPTH		(113)
#define TEST_DATA_SIZE		(TEST_DATA_WIDTH * TEST_DATA_HEIGHT * TEST_DATA_DEPTH * 2)

#define COLORMAP_PATH	"../hsv.bin"
#define COLORMAP_WIDTH	(2048)

int main(int argc, char** argv)
{
	FILE* fp = NULL;
	unsigned char* data = NULL;
	vv_context* context = NULL;
	vv_memory* volume = NULL;
	vv_memory* colormap = NULL;
	vv_visualizer* visualizer = NULL;

	fp = fopen(TEST_DATA_PATH, "r");
	data = calloc(TEST_DATA_SIZE, 1);
	fread(data, 1, TEST_DATA_SIZE, fp);
	fclose(fp);

	vv_context_create(&context);

	vv_memory_create
	(
		&volume,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = context,
			.width = TEST_DATA_WIDTH,
			.height = TEST_DATA_HEIGHT,
			.depth = TEST_DATA_DEPTH,
			.bytes_per_channel = 2,
		},
		data
	);

	free(data);

	fp = fopen(COLORMAP_PATH, "r");
	data = calloc(4 * COLORMAP_WIDTH, 1);
	fread(data, 4 * COLORMAP_WIDTH, 1, fp);
	fclose(fp);

	vv_memory_create
	(
		&colormap,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = context,
			.width = COLORMAP_WIDTH,
			.bytes_per_channel = 4,
		},
		data
	);

	free(data);

	vv_visualizer_create(context, &visualizer, VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED);
	vv_visualizer_set_viewport(visualizer, 800, 800);
	vv_visualizer_set_volume(visualizer, volume);
	vv_visualizer_set_colormap(visualizer, colormap);
	vv_visualizer_render(visualizer);

	vv_visualizer_destroy(&visualizer);
	vv_memory_destroy(&volume);
	vv_context_destroy(&context);
	return EXIT_SUCCESS;
}
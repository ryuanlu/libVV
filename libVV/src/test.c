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


vv_memory* load_texture(const vv_context* context, const char* filename, const int width, const int height, const int depth, const int bpp)
{
	int size = width * height * depth * bpp;
	char* data = NULL;
	vv_memory* texture = NULL;
	FILE* fp = NULL;


	fp = fopen(filename, "r");
	data = calloc(size, 1);
	fread(data, size, 1, fp);
	fclose(fp);

	vv_memory_create
	(
		&texture,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = context,
			.width = width,
			.height = height,
			.depth = depth,
			.bytes_per_channel = bpp,
		},
		data
	);

	free(data);

	return texture;
}


int main(int argc, char** argv)
{
	vv_context* context = NULL;
	vv_memory* volume = NULL;
	vv_memory* colormap = NULL;
	vv_visualizer* visualizer = NULL;

	vv_context_create(&context);

	volume = load_texture(context, TEST_DATA_PATH, TEST_DATA_WIDTH, TEST_DATA_HEIGHT, TEST_DATA_DEPTH, 2);
	colormap = load_texture(context, COLORMAP_PATH, COLORMAP_WIDTH, 1, 1, 4);

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

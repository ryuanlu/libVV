#include <stdio.h>
#include <stdlib.h>
#include "VV.h"

int main(int argc, char** argv)
{
	vv_context* context = NULL;
	vv_memory* memory = NULL;
	void* ptr = NULL;

	vv_context_create(&context);

	vv_memory_create
	(
		&memory,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_HOST_MEMORY,
			.width = 256,
			.height = 256,
			.bytes_per_channel = 2,
		},
		NULL
	);

	vv_memory_destroy(&memory);

	vv_memory_create
	(
		&memory,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = context,
			.width = 256,
			.height = 256,
			.bytes_per_channel = 2,
		},
		NULL
	);

	vv_memory_destroy(&memory);

	vv_memory_create
	(
		&memory,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_CL_BUFFER,
			.context = context,
			.width = 256,
			.height = 256,
			.bytes_per_channel = 2,
		},
		NULL
	);

	vv_memory_map(memory, &ptr);
	fprintf(stderr, "ptr = %p\n", ptr);
	vv_memory_unmap(memory, &ptr);
	fprintf(stderr, "ptr = %p\n", ptr);
	vv_memory_destroy(&memory);

	{
		vv_visualizer* visualizer = NULL;
		vv_visualizer_create(context, &visualizer, VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED);
		vv_visualizer_set_viewport(visualizer, 800, 800);
		vv_visualizer_destroy(&visualizer);
	}

	vv_context_destroy(&context);
	return EXIT_SUCCESS;
}
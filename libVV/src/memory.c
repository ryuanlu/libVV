#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VV.h"
#include "memory.h"
#include "gles.h"
#include "cl.h"

#define if_failed(expr, error_code, goto_label) if(!(expr)) { fprintf(stderr, "%s:%d:\t%s() returns %d\n", __FILE__, __LINE__, __FUNCTION__, error_code); result = error_code; goto goto_label; }


static int host_memory_create(struct vv_memory* memory, void* extra)
{
	memory->data = calloc(1, memory->desc.slice_pitch * memory->desc.depth * memory->desc.bytes_per_channel);
	return memory->data == NULL;
}


static int host_memory_destroy(struct vv_memory* memory)
{
	free(memory->data);
	return 0;
}


static int host_pointer_create(struct vv_memory* memory, void* extra)
{
	memory->data = extra;
	return extra == NULL;
}


static int host_pointer_destroy(struct vv_memory* memory)
{
	return 0;
}


static PFN_vv_memory_create vv_memory_create_table[] =
{
	host_memory_create,
	host_pointer_create,
	gles_texture_create,
	NULL,
	cl_buffer_create,
};


static PFN_vv_memory_destroy vv_memory_destroy_table[] =
{
	host_memory_destroy,
	host_pointer_destroy,
	gles_texture_destroy,
	NULL,
	cl_buffer_destroy,
};


int vv_memory_create(struct vv_memory** memory, const struct vv_memory_desc* desc, void* extra)
{
	int result = 0;
	struct vv_memory* new_memory = NULL;

	if_failed(memory && desc, 1, done);
	if_failed(desc->type >= 0 && desc->type < NUMBER_OF_VV_MEMORY_TYPES, 1, done);
	if_failed(desc->width > 0, 1, done);
	if_failed((desc->type <= VV_MEMORY_TYPE_HOST_POINTER) || (desc->type >= VV_MEMORY_TYPE_GLES_TEXTURE && desc->context), 1, done);

	new_memory = calloc(1, sizeof(struct vv_memory));

	memcpy(&new_memory->desc, desc, sizeof(struct vv_memory_desc));

	new_memory->desc.context = desc->context;

	new_memory->desc.height = desc->height > 0 ? desc->height : 1;
	new_memory->desc.depth = desc->depth > 0 ? desc->depth : 1;
	new_memory->desc.bytes_per_channel = desc->bytes_per_channel > 0 ? desc->bytes_per_channel : 1;

	new_memory->desc.row_pitch = desc->row_pitch > 0 ? desc->row_pitch : desc->width * new_memory->desc.bytes_per_channel;
	new_memory->desc.slice_pitch = desc->slice_pitch > 0 ? desc->slice_pitch : new_memory->desc.row_pitch * new_memory->desc.height;

	if_failed(!vv_memory_create_table[desc->type](new_memory, extra), 2, new_memory_cleanup);

	*memory = new_memory;
	goto done;

new_memory_cleanup:
	free(new_memory);
done:
	return result;
}


int vv_memory_destroy(vv_memory** memory)
{
	int result = 0;

	if_failed(memory, 1, done);
	vv_memory_destroy_table[(*memory)->desc.type](*memory);
	free(*memory);
	*memory = NULL;
done:
	return result;
}



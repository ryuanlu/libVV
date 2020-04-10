#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VV.h"
#include "memory.h"
#include "gles.h"
#include "cl.h"
#include "debug.h"


static enum vv_result host_memory_create(struct vv_memory* memory, void* extra)
{
	memory->data = calloc(1, memory->desc.slice_pitch * memory->desc.depth * memory->desc.bytes_per_channel);
	return memory->data ? VV_SUCCESS : VV_BAD_ALLOCATION;
}


static enum vv_result host_memory_destroy(struct vv_memory* memory)
{
	free(memory->data);
	return VV_SUCCESS;
}


static enum vv_result host_memory_map(const struct vv_memory* memory, void** ptr)
{
	*ptr = memory->data;
	return memory->data ? VV_SUCCESS : VV_INVALID_VALUE;
}


static enum vv_result host_memory_unmap(const struct vv_memory* memory, void** ptr)
{
	*ptr = NULL;
	return VV_SUCCESS;
}


static enum vv_result host_pointer_create(struct vv_memory* memory, void* extra)
{
	memory->data = extra;
	return extra ? VV_SUCCESS : VV_INVALID_VALUE;
}


static enum vv_result host_pointer_destroy(struct vv_memory* memory)
{
	return VV_SUCCESS;
}


static enum vv_result mapable_duplicate(struct vv_memory** memory, const struct vv_memory_desc* desc, const struct vv_memory* source)
{
	enum vv_result result = VV_SUCCESS;
	struct vv_memory* new_memory = NULL;
	char* src_data = NULL;
	char* dest_data = NULL;
	const struct vv_memory_desc* src_desc = &source->desc;
	int d, h;

	goto_cleanup_if_failed(vv_memory_create(&new_memory, desc, NULL), done);

	goto_cleanup_if_failed(vv_memory_map(source, (void**)&src_data), new_memory_cleanup);
	goto_cleanup_if_failed(vv_memory_map(new_memory, (void**)&dest_data), new_memory_cleanup);

	for(d = 0;d < desc->depth;++d)
	{
		for(h = 0;h < desc->height;++h)
		{
			memcpy
			(
				&dest_data[d * desc->slice_pitch + h * desc->row_pitch],
				&src_data[d * src_desc->slice_pitch + h * src_desc->row_pitch],
				desc->width * desc->bytes_per_channel
			);
		}
	}

	vv_memory_unmap(source, (void**)&src_data);
	vv_memory_unmap(new_memory, (void**)&dest_data);

	*memory = new_memory;
	goto done;

new_memory_cleanup:
	vv_memory_destroy(&new_memory);
done:
	return result;
}


static PFN_vv_memory_create vv_memory_create_table[] =
{
	host_memory_create,
	host_pointer_create,
	gles_texture_create,
	NULL,
	cl_buffer_create,
	NULL,
};


static PFN_vv_memory_destroy vv_memory_destroy_table[] =
{
	host_memory_destroy,
	host_pointer_destroy,
	gles_texture_destroy,
	NULL,
	cl_buffer_destroy,
	NULL,
};


static PFN_vv_memory_map vv_memory_map_table[] =
{
	host_memory_map,
	host_memory_map,
	NULL,
	NULL,
	cl_buffer_map,
	NULL,
};


static PFN_vv_memory_unmap vv_memory_unmap_table[] =
{
	host_memory_unmap,
	host_memory_unmap,
	NULL,
	NULL,
	cl_buffer_unmap,
	NULL,
};


static PFN_vv_memory_duplicate vv_memory_duplicate_table[] =
{
	mapable_duplicate,
	mapable_duplicate,
	NULL,
	NULL,
	mapable_duplicate,
	NULL,

	mapable_duplicate,
	mapable_duplicate,
	NULL,
	NULL,
	mapable_duplicate,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,

	mapable_duplicate,
	mapable_duplicate,
	NULL,
	NULL,
	mapable_duplicate,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};


enum vv_result vv_memory_create(struct vv_memory** memory, const struct vv_memory_desc* desc, void* extra)
{
	enum vv_result result = VV_SUCCESS;
	struct vv_memory* new_memory = NULL;

	goto_cleanup_if(!memory || !desc, VV_INVALID_VALUE, done);
	goto_cleanup_if(desc->type < 0 || desc->type >= NUMBER_OF_VV_MEMORY_TYPES, VV_INVALID_VALUE, done);
	goto_cleanup_if(desc->width <= 0, VV_INVALID_VALUE, done);
	goto_cleanup_if(desc->type >= VV_MEMORY_TYPE_GLES_TEXTURE && !desc->context, VV_INVALID_CONTEXT, done);

	new_memory = calloc(1, sizeof(struct vv_memory));

	memcpy(&new_memory->desc, desc, sizeof(struct vv_memory_desc));

	new_memory->desc.context = desc->context;

	new_memory->desc.height = desc->height > 0 ? desc->height : 1;
	new_memory->desc.depth = desc->depth > 0 ? desc->depth : 1;
	new_memory->desc.bytes_per_channel = desc->bytes_per_channel > 0 ? desc->bytes_per_channel : 1;

	new_memory->desc.row_pitch = desc->row_pitch > 0 ? desc->row_pitch : desc->width * new_memory->desc.bytes_per_channel;
	new_memory->desc.slice_pitch = desc->slice_pitch > 0 ? desc->slice_pitch : new_memory->desc.row_pitch * new_memory->desc.height;

	if(vv_memory_create_table[desc->type])
		goto_cleanup_if_failed(vv_memory_create_table[desc->type](new_memory, extra), new_memory_cleanup);

	*memory = new_memory;
	goto done;

new_memory_cleanup:
	free(new_memory);
done:
	return result;
}


enum vv_result vv_memory_destroy(vv_memory** memory)
{
	enum vv_result result = VV_SUCCESS;

	goto_cleanup_if(!memory, VV_INVALID_VALUE, done);

	if(vv_memory_destroy_table[(*memory)->desc.type])
		vv_memory_destroy_table[(*memory)->desc.type](*memory);

	free(*memory);
	*memory = NULL;
done:
	return result;
}


enum vv_result vv_memory_map(const struct vv_memory* memory, void** ptr)
{
	return vv_memory_map_table[memory->desc.type] ? vv_memory_map_table[memory->desc.type](memory, ptr) : VV_OPERATION_NOT_SUPPORTED;
}


enum vv_result vv_memory_unmap(const struct vv_memory* memory, void** ptr)
{
	return vv_memory_unmap_table[memory->desc.type] ? vv_memory_unmap_table[memory->desc.type](memory, ptr) : VV_OPERATION_NOT_SUPPORTED;
}


enum vv_result vv_memory_duplicate(struct vv_memory** memory, const struct vv_memory_desc* desc, const struct vv_memory* source)
{
	return vv_memory_duplicate_table[NUMBER_OF_VV_MEMORY_TYPES * source->desc.type + desc->type] ? vv_memory_duplicate_table[NUMBER_OF_VV_MEMORY_TYPES * desc->type + source->desc.type](memory, desc, source) : VV_OPERATION_NOT_SUPPORTED;
}

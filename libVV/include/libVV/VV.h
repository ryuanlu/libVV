#ifndef __VV_H__
#define __VV_H__

typedef struct vv_context vv_context;

int	vv_context_create	(vv_context** context);
int	vv_context_destroy	(vv_context** context);


typedef struct vv_memory vv_memory;
typedef enum vv_memory_type
{
	VV_MEMORY_TYPE_HOST_MEMORY,
	VV_MEMORY_TYPE_HOST_POINTER,
	VV_MEMORY_TYPE_GLES_TEXTURE,
	VV_MEMORY_TYPE_GLES_BUFFER,
	VV_MEMORY_TYPE_CL_BUFFER,
	VV_MEMORY_TYPE_CL_IMAGE,
	NUMBER_OF_VV_MEMORY_TYPES,
}vv_memory_type;

typedef struct vv_memory_desc
{
	vv_memory_type		type;
	const vv_context*	context;

	int	width;
	int	height;
	int	depth;
	int	bytes_per_channel;

	int	row_pitch;
	int	slice_pitch;

}vv_memory_desc;


int	vv_memory_create	(vv_memory** memory, const vv_memory_desc* desc, void* extra);
int	vv_memory_destroy	(vv_memory** memory);


#endif /* __VV_H__ */
#ifndef __VV_H__
#define __VV_H__


typedef enum vv_result
{
	VV_SUCCESS,
	VV_FAILED_TO_INITIALIZE,
	VV_INVALID_VALUE,
	VV_INVALID_CONTEXT,
	VV_OPERATION_NOT_SUPPORTED,
	VV_BAD_ALLOCATION,

}vv_result;


/* vv_context */

typedef struct vv_context vv_context;

vv_result	vv_context_create	(vv_context** context);
vv_result	vv_context_destroy	(vv_context** context);
vv_result	vv_context_get_eglcontext(const vv_context* context, void** eglcontext);

/* vv_memory */

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

vv_result	vv_memory_create	(vv_memory** memory, const vv_memory_desc* desc, void* extra);
vv_result	vv_memory_destroy	(vv_memory** memory);

vv_result	vv_memory_map	(const vv_memory* memory, void** ptr);
vv_result	vv_memory_unmap	(const vv_memory* memory, void** ptr);

vv_result	vv_memory_duplicate	(vv_memory** memory, const vv_memory_desc* desc, const vv_memory* source);

/* vv_visualizer */

typedef struct vv_visualizer vv_visualizer;
typedef enum vv_visualizer_type
{
	VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED,
	VV_VISUALIZER_TYPE_3D_TEXTURE_VIEW_ALIGNED,
	NUMBER_OF_VV_VISUALIZER_TYPES,
}vv_visualizer_type;

vv_result	vv_visualizer_create	(vv_context* context, vv_visualizer** visualizer, const vv_visualizer_type type);
vv_result	vv_visualizer_destroy	(vv_visualizer** visualizer);

vv_result	vv_visualizer_set_viewport	(vv_visualizer* visualizer, const int width, const int height);
vv_result	vv_visualizer_set_volume	(vv_visualizer* visualizer, vv_memory* volume);
vv_result	vv_visualizer_set_colormap	(vv_visualizer* visualizer, vv_memory* colormap);

vv_result	vv_visualizer_render	(vv_visualizer* visualizer);
vv_result	vv_visualizer_get_pixels(vv_visualizer* visualizer, char* pixels);



#endif /* __VV_H__ */
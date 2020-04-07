#ifndef __VISUALIZER_H__
#define __VISUALIZER_H__

#include "VV.h"

struct vv_visualizer
{
	enum vv_visualizer_type	type;
	struct vv_context*	context;
	struct vv_memory*	framebuffer;

	struct vv_memory*	volume;
	struct vv_memory*	colormap;

	float	modelview[16];
	float	projection[16];

	void*	derivative;
};



typedef enum vv_result (*PFN_vv_visualizer_create)(struct vv_visualizer* visualizer);
typedef enum vv_result (*PFN_vv_visualizer_destroy)(struct vv_visualizer* visualizer);

typedef enum vv_result (*PFN_vv_visualizer_set_viewport)(struct vv_visualizer* visualizer, const int width, const int height);
typedef enum vv_result (*PFN_vv_visualizer_set_volume)(struct vv_visualizer* visualizer, struct vv_memory* volume);
typedef enum vv_result (*PFN_vv_visualizer_set_colormap)(struct vv_visualizer* visualizer, struct vv_memory* colormap);

typedef enum vv_result (*PFN_vv_visualizer_render)(struct vv_visualizer* visualizer);


#endif /* __VISUALIZER_H__ */
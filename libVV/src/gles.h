#ifndef __GLES_H__
#define __GLES_H__

#include <EGL/egl.h>
#include "memory.h"

struct gles_context
{
	EGLDisplay	display;
	EGLContext	context;
	EGLConfig	config;
};


enum vv_result	gles_context_create	(struct gles_context** context);
enum vv_result	gles_context_destroy	(struct gles_context** context);

enum vv_result	gles_texture_create	(struct vv_memory* memory, void* extra);
enum vv_result	gles_texture_destroy	(struct vv_memory* memory);

#endif /* __GLES_H__ */
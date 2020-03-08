#ifndef __GLES_H__
#define __GLES_H__

#include <EGL/egl.h>


struct gles_context
{
	EGLDisplay	display;
	EGLContext	context;
	EGLConfig	config;
};


int	gles_context_create(struct gles_context** context);
int	gles_context_destroy(struct gles_context** context);


#endif /* __GLES_H__ */
#include <stdio.h>
#include <stdlib.h>
#include "gles.h"


int gles_context_create(struct gles_context** context)
{
	struct gles_context* new_context = NULL;
	int num_config = 0;

	EGLint major_version, minor_version;

	const EGLint attrib_list[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

	new_context = calloc(sizeof(struct gles_context), 1);

	new_context->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(new_context->display, &major_version, &minor_version);
	eglChooseConfig(new_context->display, attrib_list, &new_context->config, 1, &num_config);
	eglBindAPI(EGL_OPENGL_ES_API);
	new_context->context = eglCreateContext(new_context->display, new_context->config, EGL_NO_CONTEXT, NULL);

	eglMakeCurrent(new_context->display, EGL_NO_SURFACE, EGL_NO_SURFACE, new_context->context);

	*context = new_context;

	return 0;
}


int gles_context_destroy(struct gles_context** context)
{
	eglMakeCurrent((*context)->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext((*context)->display, (*context)->context);
	eglTerminate((*context)->display);

	free(*context);
	*context = NULL;

	return 0;
}


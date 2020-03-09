#include <stdio.h>
#include <stdlib.h>
#include "gles.h"

#define if_failed(expr, error_code, goto_label) if(!(expr)) { fprintf(stderr, "%s:%d:\t%s() returns %d\n", __FILE__, __LINE__, __FUNCTION__, error_code); result = error_code; goto goto_label; }


int gles_context_create(struct gles_context** context)
{
	int result = 0;

	struct gles_context* new_context = NULL;
	int num_config = 0;

	EGLint major_version = 0, minor_version = 0;

	const EGLint attrib_list[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

	if_failed(context, 1, done);

	new_context = calloc(1, sizeof(struct gles_context));

	new_context->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if_failed(new_context->display, 2, new_context_cleanup);

	eglInitialize(new_context->display, &major_version, &minor_version);
	eglChooseConfig(new_context->display, attrib_list, &new_context->config, 1, &num_config);
	eglBindAPI(EGL_OPENGL_ES_API);
	new_context->context = eglCreateContext(new_context->display, new_context->config, EGL_NO_CONTEXT, NULL);

	if_failed(new_context->context, 3, egldisplay_cleanup);

	eglMakeCurrent(new_context->display, EGL_NO_SURFACE, EGL_NO_SURFACE, new_context->context);

	if_failed(eglGetError() == EGL_SUCCESS, 4, eglcontext_cleanup);

	*context = new_context;

	goto done;

eglcontext_cleanup:
	eglDestroyContext(new_context->display, new_context->context);
egldisplay_cleanup:
	eglTerminate(new_context->display);
new_context_cleanup:
	free(new_context);
done:
	return result;
}


int gles_context_destroy(struct gles_context** context)
{
	int result = 0;

	if_failed(context, 1, done);
	if_failed((*context)->display, 2, done);

	eglMakeCurrent((*context)->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext((*context)->display, (*context)->context);
	eglTerminate((*context)->display);

	free(*context);
	*context = NULL;
done:
	return result;
}


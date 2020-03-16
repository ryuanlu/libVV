#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl32.h>
#include "gles.h"
#include "debug.h"


enum vv_result gles_context_create(struct gles_context** context)
{
	enum vv_result result = VV_SUCCESS;

	struct gles_context* new_context = NULL;
	int num_config = 0;

	EGLint major_version = 0, minor_version = 0;

	const EGLint attrib_list[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

	goto_cleanup_if(!context, VV_INVALID_VALUE, done);

	new_context = calloc(1, sizeof(struct gles_context));

	new_context->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	goto_cleanup_if(!new_context->display, VV_FAILED_TO_INITIALIZE, new_context_cleanup);

	eglInitialize(new_context->display, &major_version, &minor_version);
	eglChooseConfig(new_context->display, attrib_list, &new_context->config, 1, &num_config);
	eglBindAPI(EGL_OPENGL_ES_API);
	new_context->context = eglCreateContext(new_context->display, new_context->config, EGL_NO_CONTEXT, NULL);

	goto_cleanup_if(!new_context->context, VV_FAILED_TO_INITIALIZE, egldisplay_cleanup);

	eglMakeCurrent(new_context->display, EGL_NO_SURFACE, EGL_NO_SURFACE, new_context->context);

	goto_cleanup_if(eglGetError() != EGL_SUCCESS, VV_INVALID_CONTEXT, eglcontext_cleanup);

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


enum vv_result gles_context_destroy(struct gles_context** context)
{
	enum vv_result result = VV_SUCCESS;

	goto_cleanup_if(!context, VV_INVALID_VALUE, done);
	goto_cleanup_if(!(*context)->display, VV_INVALID_CONTEXT, done);

	eglMakeCurrent((*context)->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext((*context)->display, (*context)->context);
	eglTerminate((*context)->display);

	free(*context);
	*context = NULL;
done:
	return result;
}


enum vv_result gles_texture_create(struct vv_memory* memory, void* extra)
{
	enum vv_result result = VV_SUCCESS;

	glGenTextures(1, (GLuint*)&memory->data);
	glBindTexture(GL_TEXTURE_3D, (GLuint64)memory->data);

	switch(memory->desc.bytes_per_channel)
	{
	case 1:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, memory->desc.width, memory->desc.height, memory->desc.depth, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, extra);
		break;
	case 2:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R16UI, memory->desc.width, memory->desc.height, memory->desc.depth, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, extra);
		break;
	default:
		goto_cleanup_if(1, VV_OPERATION_NOT_SUPPORTED, texture_cleanup);
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	goto done;

texture_cleanup:
	glDeleteTextures(1, (GLuint*)&memory->data);
done:
	return result;
}


enum vv_result gles_texture_destroy(struct vv_memory* memory)
{
	enum vv_result result = VV_SUCCESS;

	glBindTexture(GL_TEXTURE_3D, 0);
	goto_cleanup_if(!memory, VV_INVALID_VALUE, done);
	glDeleteTextures(1, (GLuint*)&memory->data);
done:
	return result;
}

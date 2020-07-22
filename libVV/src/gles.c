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

	const EGLint egl_attrib_list[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

	const EGLint gles_attrib_list[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 2,
		EGL_NONE
	};

	goto_cleanup_if(!context, VV_INVALID_VALUE, done);

	new_context = calloc(1, sizeof(struct gles_context));

	new_context->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	goto_cleanup_if(!new_context->display, VV_FAILED_TO_INITIALIZE, new_context_cleanup);

	eglInitialize(new_context->display, &major_version, &minor_version);
	eglChooseConfig(new_context->display, egl_attrib_list, &new_context->config, 1, &num_config);
	eglBindAPI(EGL_OPENGL_ES_API);
	new_context->context = eglCreateContext(new_context->display, new_context->config, EGL_NO_CONTEXT, gles_attrib_list);

	goto_cleanup_if(!new_context->context, VV_FAILED_TO_INITIALIZE, egldisplay_cleanup);

#ifdef CONFIG_GLES20_COMPATIBLE
	new_context->glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)eglGetProcAddress("glFramebufferTexture");
#endif
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
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, memory->desc.width, memory->desc.height, memory->desc.depth, 0, GL_RED, GL_UNSIGNED_BYTE, extra);
		break;
	case 2:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RG, memory->desc.width, memory->desc.height, memory->desc.depth, 0, GL_RG, GL_UNSIGNED_BYTE, extra);
		break;
	case 4:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, memory->desc.width, memory->desc.height, memory->desc.depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, extra);
		break;
	default:
		goto_cleanup_if(1, VV_OPERATION_NOT_SUPPORTED, texture_cleanup);
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	goto_cleanup_if(glGetError(), VV_BAD_ALLOCATION, texture_cleanup);

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


static const char* gles_shader_name[] =
{
	"GL_VERTEX_SHADER",
	"GL_FRAGMENT_SHADER",
	"GL_GEOMETRY_SHADER"
};

static const GLuint gles_shader_type[] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};


#define MAX_SHADER_INFO_LENGTH	(4096)

enum vv_result gles_create_shader(const struct gles_context* context, GLuint* shader, const enum gles_shader type, const char* src, const GLint length)
{
	char info[MAX_SHADER_INFO_LENGTH] = {0};
	int info_length = 0;
	GLuint new_shader = 0;
	enum vv_result result = VV_SUCCESS;

	goto_cleanup_if(!context || !shader || !src, VV_INVALID_VALUE, done);
	goto_cleanup_if(type < 0 || type >= NUMBER_OF_GLES_SHADERS, VV_INVALID_VALUE, done);

	new_shader = glCreateShader(gles_shader_type[type]);
	glShaderSource(new_shader, 1, &src, &length);
	goto_cleanup_if(glGetError(), VV_FAILED_TO_INITIALIZE, new_shader_cleanup);
	glCompileShader(new_shader);
	glGetShaderInfoLog(new_shader, MAX_SHADER_INFO_LENGTH, &info_length, info);

	if(info_length)
		fprintf(stderr, "%s information:\n\n%s\n", gles_shader_name[type], info);

	goto_cleanup_if(info_length, VV_FAILED_TO_INITIALIZE, new_shader_cleanup);

	*shader = new_shader;
	goto done;

new_shader_cleanup:
	glDeleteShader(new_shader);
done:
	return result;
}


enum vv_result gles_create_program(const struct gles_context* context, GLuint* program, const GLuint vertex_shader, const GLuint fragment_shader)
{
	char info[MAX_SHADER_INFO_LENGTH] = {0};
	int info_length = 0;

	GLuint new_program = 0;
	enum vv_result result = VV_SUCCESS;

	new_program = glCreateProgram();
	glAttachShader(new_program, vertex_shader);
	glAttachShader(new_program, fragment_shader);
	glLinkProgram(new_program);

	glGetProgramInfoLog(new_program, MAX_SHADER_INFO_LENGTH, &info_length, info);

	if(info_length)
		fprintf(stderr, "GL program information:\n\n%s\n", info);

	goto_cleanup_if(info_length, VV_FAILED_TO_INITIALIZE, new_program_cleanup);

	*program = new_program;
	goto done;

new_program_cleanup:
	glDeleteProgram(new_program);
done:
	return result;
}


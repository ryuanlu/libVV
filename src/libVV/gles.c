#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "gles.h"

const EGLint* gles_get_egl_config_attributes(void)
{
	static const EGLint config_attributes[] =
	{
		EGL_RENDERABLE_TYPE,		EGL_OPENGL_ES3_BIT,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BUFFER_SIZE,		24,
		EGL_DEPTH_SIZE,			24,
		EGL_NONE,
	};

	return config_attributes;
}

const EGLint* gles_get_egl_context_attributes(void)
{
	static const EGLint context_attributes[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 2,
		EGL_NONE
	};

	return context_attributes;
}

EGLConfig gles_get_eglconfig(EGLDisplay display)
{
	EGLConfig configs[32];
	EGLConfig result = NULL;
	int nr_configs;

	eglChooseConfig(display, gles_get_egl_config_attributes(), configs, 32, &nr_configs);

	for(int i = 0;i < nr_configs;++i)
	{
		int red, alpha, depth, samples, stencil;

		eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red);
		eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha);
		eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth);
		eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples);
		eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil);

		if(red != 8 || alpha != 0 || depth < 16 || result)
		{
			continue;
		}else
		{
			result = configs[i];
			break;
		}
	}

	return result;
}


GLuint gles_create_shader(const char* src, const int length, GLenum type)
{
	GLuint shader = 0;
	GLsizei msg_length = 0;
	GLint maxlength = 0;
	char* msg = NULL;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, &length);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxlength);
	msg = calloc(1, maxlength);
	glGetShaderInfoLog(shader, maxlength, &msg_length, msg);

	if(msg_length > 0)
		fprintf(stderr, "%s\n", msg);

	free(msg);

	return shader;
}

GLuint gles_create_and_link_program(const GLuint vert_shader, const GLuint frag_shader)
{
	GLuint program = 0;
	GLsizei msg_length = 0;
	GLint maxlength = 0;
	char* msg = NULL;

	program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxlength);
	msg = calloc(1, maxlength);
	glGetProgramInfoLog(program, maxlength, &msg_length, msg);

	if(msg_length > 0)
		fprintf(stderr, "%s\n", msg);

	free(msg);

	return program;
}

#ifndef __GLES_H__
#define __GLES_H__

#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include "memory.h"

struct gles_context
{
	EGLDisplay	display;
	EGLContext	context;
	EGLConfig	config;
#ifdef CONFIG_GLES20_COMPATIBLE
	PFNGLFRAMEBUFFERTEXTUREPROC	glFramebufferTexture;
#endif
};


enum vv_result	gles_context_create	(struct gles_context** context);
enum vv_result	gles_context_destroy	(struct gles_context** context);

enum vv_result	gles_texture_create	(struct vv_memory* memory, void* extra);
enum vv_result	gles_texture_destroy	(struct vv_memory* memory);


enum gles_shader
{
	VV_VERTEX_SHADER,
	VV_FRAGMENT_SHADER,
	VV_GEOMETRY_SHADER,
	NUMBER_OF_GLES_SHADERS,
};

enum vv_result	gles_create_shader	(const struct gles_context* context, GLuint* shader, const enum gles_shader type, const char* src, const GLint length);
enum vv_result	gles_create_program	(const struct gles_context* context, GLuint* program, const GLuint vertex_shader, const GLuint fragment_shader);



#endif /* __GLES_H__ */
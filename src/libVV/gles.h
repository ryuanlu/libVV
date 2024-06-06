#ifndef __GLES_H__
#define __GLES_H__

#include <GLES3/gl32.h>
#include <EGL/egl.h>

const EGLint* gles_get_egl_config_attributes(void);
const EGLint* gles_get_egl_context_attributes(void);

EGLConfig gles_get_eglconfig(EGLDisplay display);
GLuint gles_create_shader(const char* src, const int length, GLenum type);
GLuint gles_create_and_link_program(const GLuint vert_shader, const GLuint frag_shader);

#endif /* __GLES_H__ */
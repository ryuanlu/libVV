#ifndef __ISOSURFACE_RENDERER_H__
#define __ISOSURFACE_RENDERER_H__

#include "renderer.h"
#include "vertex_buffer.h"

struct isosurface_renderer
{
	struct
	{
		GLuint	gradient_texture;
		GLuint	color;
	}location;

	struct vertex_buffer* vertex_buffer;
	GLuint	vao;
	GLuint	vbo;

	struct renderer	renderer;
};

struct isosurface_renderer* isosurface_renderer_create(void);
void isosurface_renderer_destroy(struct isosurface_renderer* isosurface_renderer);

void isosurface_renderer_set_isovalue(struct isosurface_renderer* isosurface_renderer, const int isovalue);
void isosurface_renderer_set_color(struct isosurface_renderer* isosurface_renderer, const float r, const float g, const float b, const float a);


#endif /* __ISOSURFACE_RENDERER_H__ */
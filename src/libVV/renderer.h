#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <GLES3/gl32.h>

struct volume_texture;
struct renderer;

typedef int (*PFN_RENDERER_INIT_SHADERS)(struct renderer* renderer);
typedef int (*PFN_RENDERER_ATTACH_VOLUME)(struct renderer* renderer, const struct volume_texture* volume_texture);
typedef int (*PFN_RENDERER_REDRAW)(struct renderer* renderer);
typedef int (*PFN_RENDERER_RESIZE)(struct renderer* renderer, const int width, const int height);

struct renderer
{
	const struct volume_texture*	volume_texture;

	GLuint	vert_shader;
	GLuint	frag_shader;
	GLuint	program;

	struct
	{
		GLuint	scaling;
		GLuint	world_matrix;
		GLuint	viewing_matrix;
		GLuint	projection_matrix;

		GLuint	ambient;
	}location;

	float	world_matrix[16];
	float	viewing_matrix[16];
	float	background_color[4];

	PFN_RENDERER_INIT_SHADERS	init_shaders;
	PFN_RENDERER_ATTACH_VOLUME	attach_volume;
	PFN_RENDERER_REDRAW		redraw;
	PFN_RENDERER_RESIZE		resize;
};

int renderer_init(struct renderer* renderer, PFN_RENDERER_INIT_SHADERS init_shaders);
int renderer_release(struct renderer* renderer);
int renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture);

int renderer_redraw(struct renderer* renderer);
int renderer_resize(struct renderer* renderer, const int width, const int height);
float* renderer_get_world_matrix(struct renderer* renderer);
void renderer_set_background_color(struct renderer* renderer, const float red, const float green, const float blue, const float alpha);
void renderer_set_ambient(struct renderer* renderer, const float ambient);

#endif /* __RENDERER_H__ */
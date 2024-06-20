#ifndef __VOLUME_RENDERER_H__
#define __VOLUME_RENDERER_H__

#include <GLES3/gl32.h>
#include "renderer.h"

struct volume_renderer
{
	struct
	{
		GLuint	volume_texture;
		GLuint	gradient_texture;
		GLuint	colormap_texture;
		GLuint	maxvalue;
		GLuint	number_of_slices;
		GLuint	direction;
		GLuint	enable_lighting;
		GLuint	enable_auto_opacity;
	}location;

	GLuint	colormap_texture;

	float	slicing_ratio;
	int	nr_x_slices;
	int	nr_y_slices;
	int	nr_z_slices;

	struct renderer	renderer;

};

struct volume_renderer* volume_renderer_create(void);
void volume_renderer_destroy(struct volume_renderer* volume_renderer);

int volume_renderer_set_colormap(struct volume_renderer* volume_renderer, const unsigned char* colormap, const int width, const int height);
void volume_renderer_set_slicing_ratio(struct volume_renderer* renderer, const float slicing_ratio);
void volume_renderer_set_lighting(struct volume_renderer* renderer, const int enable);
void volume_renderer_set_auto_opacity(struct volume_renderer* renderer, const int enable);

#endif /* __VOLUME_RENDERER_H__ */

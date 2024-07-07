#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "renderer.h"
#include "volume_renderer.h"
#include "volume_texture.h"
#include "volume.h"
#include "gles.h"
#include "matrix.h"

#define container_of(ptr, type, member) (type *)((char *)(ptr) - (char *) &((type *)0)->member)

#define TEXTURE_INDEX_VOLUME	(0)
#define TEXTURE_INDEX_GRADIENT	(1)
#define TEXTURE_INDEX_COLORMAP	(2)
#define DEFAULT_SLICING_RATIO	(2.0f)

extern const char _binary_volume_vert_start;
extern const char _binary_volume_vert_end;
#define _binary_volume_vert_size (int)(&_binary_volume_vert_end - &_binary_volume_vert_start)

extern const char _binary_volume_frag_start;
extern const char _binary_volume_frag_end;
#define _binary_volume_frag_size (int)(&_binary_volume_frag_end - &_binary_volume_frag_start)

static int volume_renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture);
static int volume_renderer_redraw(struct renderer* renderer);
static int volume_renderer_resize(struct renderer* renderer, const int width, const int height);


static int volume_renderer_init_shaders(struct renderer* renderer)
{
	renderer->vert_shader = gles_create_shader(&_binary_volume_vert_start, _binary_volume_vert_size, GL_VERTEX_SHADER);
	renderer->frag_shader = gles_create_shader(&_binary_volume_frag_start, _binary_volume_frag_size, GL_FRAGMENT_SHADER);
	renderer->program = gles_create_and_link_program(renderer->vert_shader, renderer->frag_shader);

	return 0;
}

static int volume_renderer_init(struct volume_renderer* volume_renderer)
{
	float identity[16];

	struct renderer* renderer = &volume_renderer->renderer;
	memset(volume_renderer, 0, sizeof(struct volume_renderer));
	renderer_init(&volume_renderer->renderer, volume_renderer_init_shaders);
	volume_renderer->renderer.attach_volume = volume_renderer_attach_volume;
	volume_renderer->renderer.redraw = volume_renderer_redraw;
	volume_renderer->renderer.resize = volume_renderer_resize;

	volume_renderer->location.volume_texture = glGetUniformLocation(renderer->program, "volume");
	volume_renderer->location.gradient_texture = glGetUniformLocation(renderer->program, "gradient");
	volume_renderer->location.colormap_texture = glGetUniformLocation(renderer->program, "colormap");
	volume_renderer->location.maxvalue = glGetUniformLocation(renderer->program, "maxvalue");
	volume_renderer->location.number_of_slices = glGetUniformLocation(renderer->program, "number_of_slices");
	volume_renderer->location.direction = glGetUniformLocation(renderer->program, "direction");
	volume_renderer->location.enable_lighting = glGetUniformLocation(renderer->program, "enable_lighting");
	volume_renderer->location.enable_auto_opacity = glGetUniformLocation(renderer->program, "enable_auto_opacity");

	mat4_set_identity(identity);
	glUniformMatrix4fv(volume_renderer->location.direction, 1, 0, identity);

	glUniform1i(volume_renderer->location.volume_texture, TEXTURE_INDEX_VOLUME);
	glUniform1i(volume_renderer->location.gradient_texture, TEXTURE_INDEX_GRADIENT);
	glUniform1i(volume_renderer->location.colormap_texture, TEXTURE_INDEX_COLORMAP);
	glUniform1i(volume_renderer->location.maxvalue, 0);
	glUniform1i(volume_renderer->location.number_of_slices, 0);
	glUniform1i(volume_renderer->location.enable_lighting, 0);
	glUniform1i(volume_renderer->location.enable_auto_opacity, 0);

	glGenTextures(1, &volume_renderer->colormap_texture);

	volume_renderer->slicing_ratio = DEFAULT_SLICING_RATIO;

	return 0;
}

static int volume_renderer_release(struct volume_renderer* volume_renderer)
{
	if(!volume_renderer)
		return 1;

	glDeleteTextures(1, &volume_renderer->colormap_texture);
	renderer_release(&volume_renderer->renderer);
	return 0;
}

struct volume_renderer* volume_renderer_create(void)
{
	struct volume_renderer* volume_renderer = calloc(1, sizeof(struct volume_renderer));
	volume_renderer_init(volume_renderer);
	return volume_renderer;
}

void volume_renderer_destroy(struct volume_renderer* volume_renderer)
{
	volume_renderer_release(volume_renderer);
	free(volume_renderer);
}

static int volume_renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture)
{
	if(!renderer || !volume_texture)
		return 1;

	struct volume_renderer* volume_renderer = container_of(renderer, struct volume_renderer, renderer);

	glUniform1i(volume_renderer->location.maxvalue, volume_texture->volume->maxvalue);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_VOLUME);
	glBindTexture(GL_TEXTURE_3D, renderer->volume_texture->volume_texture);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_GRADIENT);
	glBindTexture(GL_TEXTURE_3D, renderer->volume_texture->gradient);

	volume_renderer->nr_x_slices = volume_texture->volume->params.widthscale * volume_texture->volume->params.width;
	volume_renderer->nr_y_slices = volume_texture->volume->params.heightscale * volume_texture->volume->params.height;
	volume_renderer->nr_z_slices = volume_texture->volume->params.depthscale * volume_texture->volume->params.depth;

	return 0;
}

int volume_renderer_set_colormap(struct volume_renderer* volume_renderer, const unsigned char* colormap, const int width, const int height)
{
	if(!volume_renderer || !colormap)
		return 1;

	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_COLORMAP);
	glBindTexture(GL_TEXTURE_2D, volume_renderer->colormap_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colormap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return 0;
}

static float calculate_cosine(const float* mat4, const float* vec4)
{
	int i = 0;
	float vec[4];
	float origin[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	mat4_multiplied_by_vec4(origin, mat4, origin);
	mat4_multiplied_by_vec4(vec, mat4, vec4);

	for(i = 0;i < 4;++i)
		vec[i] /= vec[3];

	for(i = 0;i < 4;++i)
		origin[i] /= origin[3];

	for(i = 0;i < 3;++i)
		vec[i] -= origin[i];

	vec_normalize(vec, 3);

	return vec[2];
}

static int volume_renderer_redraw(struct renderer* renderer)
{

	if(!renderer || !renderer->volume_texture)
		return 1;

	struct volume_renderer* volume_renderer = container_of(renderer, struct volume_renderer, renderer);

	float mat[16];
	float xaxis[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	float yaxis[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	float zaxis[4] = {0.0f, 0.0f, 1.0f, 1.0f};

	mat4_multiplied_by_mat4(mat, renderer->viewing_matrix, renderer->world_matrix);

	float xaxis_cosine = calculate_cosine(mat, xaxis);
	float yaxis_cosine = calculate_cosine(mat, yaxis);
	float zaxis_cosine = calculate_cosine(mat, zaxis);

	float abs_xaxis_cosine = fabs(xaxis_cosine);
	float abs_yaxis_cosine = fabs(yaxis_cosine);
	float abs_zaxis_cosine = fabs(zaxis_cosine);

	int nr_slices = 0;


	float direction[16];

	mat4_set_zero(direction);
	direction[15] = 1.0f;

	if(abs_xaxis_cosine >= abs_yaxis_cosine && abs_xaxis_cosine >= abs_zaxis_cosine)
	{
		nr_slices = volume_renderer->nr_x_slices;

		mat4_set_zero(direction);
		direction[15] = 1.0f;

		direction[8] = 1.0f * (xaxis_cosine < 0.0f ? -1.0f : 1.0f);
		direction[1] = 1.0f;
		direction[6] = 1.0f;
	}

	if(abs_yaxis_cosine >= abs_xaxis_cosine && abs_yaxis_cosine >= abs_zaxis_cosine)
	{
		nr_slices = volume_renderer->nr_y_slices;

		mat4_set_zero(direction);
		direction[15] = 1.0f;

		direction[0] = 1.0f;
		direction[9] = 1.0f * (yaxis_cosine < 0.0f ? -1.0f : 1.0f);
		direction[6] = -1.0f;
	}

	if(abs_zaxis_cosine >= abs_xaxis_cosine && abs_zaxis_cosine >= abs_yaxis_cosine)
	{
		nr_slices = volume_renderer->nr_z_slices;

		mat4_set_zero(direction);
		direction[15] = 1.0f;

		direction[0] = 1.0f;
		direction[5] = 1.0f;
		direction[10] = 1.0f * (zaxis_cosine < 0.0f ? -1.0f : 1.0f);
	}

	glUniformMatrix4fv(volume_renderer->location.direction, 1, 0, direction);
	glUniformMatrix4fv(renderer->location.world_matrix, 1, 0, renderer->world_matrix);
	glUniformMatrix4fv(renderer->location.viewing_matrix, 1, 0, renderer->viewing_matrix);

	glClearColor(renderer->background_color[0], renderer->background_color[1], renderer->background_color[2], renderer->background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	nr_slices *= volume_renderer->slicing_ratio;
	glUniform1i(volume_renderer->location.number_of_slices, nr_slices);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nr_slices);
	glFinish();

	return 0;
}

int volume_renderer_resize(struct renderer* renderer, const int width, const int height)
{
	return 0;
}

void volume_renderer_set_slicing_ratio(struct volume_renderer* volume_renderer, const float slicing_ratio)
{
	volume_renderer->slicing_ratio = slicing_ratio;
}

void volume_renderer_set_lighting(struct volume_renderer* renderer, const int enable)
{
	glUniform1i(renderer->location.enable_lighting, !!enable);
}

void volume_renderer_set_auto_opacity(struct volume_renderer* renderer, const int enable)
{
	glUniform1i(renderer->location.enable_auto_opacity, !!enable);
}

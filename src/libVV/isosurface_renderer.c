#include <stdlib.h>
#include <string.h>
#include "gles.h"
#include "isosurface_renderer.h"
#include "volume_texture.h"
#include "renderer.h"
#include "matrix.h"
#include "isosurface.h"
#include "vertex_buffer.h"

#define container_of(ptr, type, member) (type *)((char *)(ptr) - (char *) &((type *)0)->member)

#define TEXTURE_INDEX_GRADIENT	(0)

extern const char _binary_isosurface_vert_start;
extern const char _binary_isosurface_vert_end;
#define _binary_isosurface_vert_size (int)(&_binary_isosurface_vert_end - &_binary_isosurface_vert_start)

extern const char _binary_isosurface_frag_start;
extern const char _binary_isosurface_frag_end;
#define _binary_isosurface_frag_size (int)(&_binary_isosurface_frag_end - &_binary_isosurface_frag_start)

static int isosurface_renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture);
static int isosurface_renderer_redraw(struct renderer* renderer);

static int isosurface_renderer_init_shaders(struct renderer* renderer)
{
	renderer->vert_shader = gles_create_shader(&_binary_isosurface_vert_start, _binary_isosurface_vert_size, GL_VERTEX_SHADER);
	renderer->frag_shader = gles_create_shader(&_binary_isosurface_frag_start, _binary_isosurface_frag_size, GL_FRAGMENT_SHADER);
	renderer->program = gles_create_and_link_program(renderer->vert_shader, renderer->frag_shader);

	return 0;
}

static int isosurface_renderer_init(struct isosurface_renderer* isosurface_renderer)
{
	float identity[16];

	struct renderer* renderer = &isosurface_renderer->renderer;
	memset(isosurface_renderer, 0, sizeof(struct isosurface_renderer));
	renderer_init(&isosurface_renderer->renderer, isosurface_renderer_init_shaders);
	isosurface_renderer->renderer.attach_volume = isosurface_renderer_attach_volume;
	isosurface_renderer->renderer.redraw = isosurface_renderer_redraw;

	isosurface_renderer->location.gradient_texture = glGetUniformLocation(renderer->program, "gradient");
	isosurface_renderer->location.color = glGetUniformLocation(renderer->program, "color");

	mat4_set_identity(identity);

	glUniform1i(isosurface_renderer->location.gradient_texture, TEXTURE_INDEX_GRADIENT);
	glUniform4f(isosurface_renderer->location.color, 1.0f, 1.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &isosurface_renderer->vao);
	glGenBuffers(1, &isosurface_renderer->vbo);
	glBindVertexArray(isosurface_renderer->vao);
	glBindBuffer(GL_ARRAY_BUFFER, isosurface_renderer->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;
}

static int isosurface_renderer_release(struct isosurface_renderer* isosurface_renderer)
{
	if(!isosurface_renderer)
		return 1;

	if(isosurface_renderer->vao)
		glDeleteVertexArrays(1, &isosurface_renderer->vao);
	if(isosurface_renderer->vbo)
		glDeleteBuffers(1, &isosurface_renderer->vbo);

	vertex_buffer_destroy(isosurface_renderer->vertex_buffer);
	renderer_release(&isosurface_renderer->renderer);

	return 0;
}

struct isosurface_renderer* isosurface_renderer_create(void)
{
	struct isosurface_renderer* isosurface_renderer = calloc(1, sizeof(struct isosurface_renderer));
	isosurface_renderer_init(isosurface_renderer);
	return isosurface_renderer;
}

void isosurface_renderer_destroy(struct isosurface_renderer* isosurface_renderer)
{
	isosurface_renderer_release(isosurface_renderer);
	free(isosurface_renderer);
}

static int isosurface_renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture)
{
	if(!renderer || !volume_texture)
		return 1;


	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_GRADIENT);
	glBindTexture(GL_TEXTURE_3D, renderer->volume_texture->gradient);

	return 0;
}

static int isosurface_renderer_redraw(struct renderer* renderer)
{
	struct isosurface_renderer* isosurface_renderer = container_of(renderer, struct isosurface_renderer, renderer);

	glUniformMatrix4fv(renderer->location.world_matrix, 1, 0, renderer->world_matrix);
	glUniformMatrix4fv(renderer->location.viewing_matrix, 1, 0, renderer->viewing_matrix);

	glClearColor(renderer->background_color[0], renderer->background_color[1], renderer->background_color[2], renderer->background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(isosurface_renderer->vao);
	glDrawArrays(GL_TRIANGLES, 0, isosurface_renderer->vertex_buffer->count);
	glFinish();

	return 0;
}

void isosurface_renderer_set_isovalue(struct isosurface_renderer* isosurface_renderer, const int isovalue)
{
	if(!isosurface_renderer->vertex_buffer)
	{
		isosurface_renderer->vertex_buffer = vertex_buffer_create(4096);
	}

	vertex_buffer_clear(isosurface_renderer->vertex_buffer);
	isosurface_extract(isosurface_renderer->renderer.volume_texture->volume, isovalue, isosurface_renderer->vertex_buffer);

	glBindVertexArray(isosurface_renderer->vao);
	glBindBuffer(GL_ARRAY_BUFFER, isosurface_renderer->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * isosurface_renderer->vertex_buffer->count, isosurface_renderer->vertex_buffer->buffer, GL_STATIC_DRAW);

}

void isosurface_renderer_set_color(struct isosurface_renderer* isosurface_renderer, const float r, const float g, const float b, const float a)
{
	glUniform4f(isosurface_renderer->location.color, r, g, b, a);
}

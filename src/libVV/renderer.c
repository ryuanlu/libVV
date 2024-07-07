#include <stdlib.h>
#include <string.h>
#include "renderer.h"
#include "matrix.h"
#include "gles.h"
#include "volume.h"
#include "volume_texture.h"

#define DEFAULT_AMBIENT	(0.0)

int renderer_init(struct renderer* renderer, PFN_RENDERER_INIT_SHADERS init_shaders)
{
	float identity[16];

	memset(renderer, 0, sizeof(struct renderer));
	renderer->init_shaders = init_shaders;

	if(init_shaders(renderer))
		return 1;

	mat4_set_identity(identity);
	mat4_set_identity(renderer->viewing_matrix);
	mat4_set_translate(renderer->world_matrix, 0.0f, 0.0f, -1.0f);

	glUseProgram(renderer->program);

	renderer->location.scaling = glGetUniformLocation(renderer->program, "scaling");
	renderer->location.world_matrix = glGetUniformLocation(renderer->program, "world_matrix");
	renderer->location.viewing_matrix = glGetUniformLocation(renderer->program, "viewing_matrix");
	renderer->location.projection_matrix = glGetUniformLocation(renderer->program, "projection_matrix");
	renderer->location.ambient = glGetUniformLocation(renderer->program, "ambient");

	glUniformMatrix4fv(renderer->location.scaling, 1, 0, identity);
	glUniformMatrix4fv(renderer->location.world_matrix, 1, 0, renderer->world_matrix);
	glUniformMatrix4fv(renderer->location.viewing_matrix, 1, 0, renderer->viewing_matrix);
	glUniformMatrix4fv(renderer->location.projection_matrix, 1, 0, identity);
	glUniform1f(renderer->location.ambient, DEFAULT_AMBIENT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	return 0;
}


int renderer_release(struct renderer* renderer)
{
	if(!renderer)
		return 1;

	glUseProgram(0);
	glDeleteProgram(renderer->program);
	glDeleteShader(renderer->vert_shader);
	glDeleteShader(renderer->frag_shader);

	return 0;
}

int renderer_attach_volume(struct renderer* renderer, const struct volume_texture* volume_texture)
{
	if(!renderer || !volume_texture)
		return 1;

	struct volume* volume = volume_texture->volume;
	renderer->volume_texture = volume_texture;

	float vec[3] = {volume->params.widthscale * volume->params.width, volume->params.heightscale * volume->params.height, volume->params.depthscale * volume->params.depth};
	float scaling[16];

	vec_normalize(vec, 3);
	mat4_set_scaling(scaling, vec[0], vec[1], vec[2]);
	glUniformMatrix4fv(renderer->location.scaling, 1, 0, scaling);

	if(renderer->attach_volume)
		renderer->attach_volume(renderer, volume_texture);

	return 0;
}

int renderer_redraw(struct renderer* renderer)
{
	if(renderer->redraw)
		renderer->redraw(renderer);

	return 0;
}

int renderer_resize(struct renderer* renderer, const int width, const int height)
{
	float mat4_projection[16];

	mat4_set_perspective(mat4_projection, 60.0f, (float)width / (float)height, 0.1f, 5.0f);
	glUniformMatrix4fv(renderer->location.projection_matrix, 1, 0, mat4_projection);
	glViewport(0, 0, width, height);

	if(renderer->resize)
		renderer->resize(renderer, width, height);

	return 0;
}

float* renderer_get_world_matrix(struct renderer* renderer)
{
	return renderer->world_matrix;
}

void renderer_set_background_color(struct renderer* renderer, const float red, const float green, const float blue, const float alpha)
{
	renderer->background_color[0] = red;
	renderer->background_color[1] = green;
	renderer->background_color[2] = blue;
	renderer->background_color[3] = alpha;
}

void renderer_set_ambient(struct renderer* renderer, const float ambient)
{
	glUniform1f(renderer->location.ambient, ambient);
}

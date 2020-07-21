#include <stdlib.h>
#include <GLES3/gl32.h>
#include "gles.h"
#include "context.h"
#include "memory.h"
#include "visualizer.h"
#include "matrix.h"
#include "debug.h"


extern const char _binary_src_axis_aligned_vert_start;
extern const char _binary_src_axis_aligned_vert_end;
extern const char _binary_src_axis_aligned_frag_start;
extern const char _binary_src_axis_aligned_frag_end;

struct gles_visualizer
{
	struct gles_context*	context;
	struct vv_memory*	texture;
	struct vv_memory*	colormap;

	GLuint	vertex_shader;
	GLuint	fragment_shader;
	GLuint	shader_program;

	GLuint	nr_slices_loc;
	GLuint	local_loc;
	GLuint	world_loc;
	GLuint	view_loc;
	GLuint	projection_loc;
	GLuint	data_loc;
	GLuint	colormap_loc;

	GLuint	fbo;
	GLuint	vao;
};


enum vv_result gles_visualizer_create(struct vv_visualizer* visualizer)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* new_visualizer = NULL;
	const char* vert_src = NULL;
	const char* frag_src = NULL;
	const int sizeof_vert_src = &_binary_src_axis_aligned_vert_end - &_binary_src_axis_aligned_vert_start;
	const int sizeof_frag_src = &_binary_src_axis_aligned_frag_end - &_binary_src_axis_aligned_frag_start;

	new_visualizer = calloc(1, sizeof(struct gles_visualizer));
	new_visualizer->context = visualizer->context->gles;

	switch(visualizer->type)
	{
	case VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED:
		vert_src = &_binary_src_axis_aligned_vert_start;
		frag_src = &_binary_src_axis_aligned_frag_start;
		break;
	case VV_VISUALIZER_TYPE_3D_TEXTURE_VIEW_ALIGNED:
		vert_src = NULL;
		frag_src = NULL;
		break;
	default:
		break;
	}

	goto_cleanup_if_failed(gles_create_shader(new_visualizer->context, &new_visualizer->vertex_shader, VV_VERTEX_SHADER, vert_src, sizeof_vert_src), new_visualizer_cleanup);
	goto_cleanup_if_failed(gles_create_shader(new_visualizer->context, &new_visualizer->fragment_shader, VV_FRAGMENT_SHADER, frag_src, sizeof_frag_src), vertex_shader_cleanup);
	goto_cleanup_if_failed(gles_create_program(new_visualizer->context, &new_visualizer->shader_program, new_visualizer->vertex_shader, new_visualizer->fragment_shader), fragment_shader_cleanup);

	glGenFramebuffers(1, &new_visualizer->fbo);
	glGenVertexArrays(1, &new_visualizer->vao);

	new_visualizer->nr_slices_loc	= glGetUniformLocation(new_visualizer->shader_program, "nr_slices");
	new_visualizer->local_loc	= glGetUniformLocation(new_visualizer->shader_program, "local");
	new_visualizer->world_loc	= glGetUniformLocation(new_visualizer->shader_program, "world");
	new_visualizer->view_loc	= glGetUniformLocation(new_visualizer->shader_program, "view");
	new_visualizer->projection_loc	= glGetUniformLocation(new_visualizer->shader_program, "projection");
	new_visualizer->data_loc	= glGetUniformLocation(new_visualizer->shader_program, "data");
	new_visualizer->colormap_loc	= glGetUniformLocation(new_visualizer->shader_program, "colormap");

	glUseProgram(new_visualizer->shader_program);
	glUniform1i(new_visualizer->data_loc, 0);
	glUniform1i(new_visualizer->colormap_loc, 1);
	glUseProgram(0);

	visualizer->derivative = new_visualizer;
	goto done;

fragment_shader_cleanup:
	glDeleteShader(new_visualizer->fragment_shader);
vertex_shader_cleanup:
	glDeleteShader(new_visualizer->vertex_shader);
new_visualizer_cleanup:
	free(new_visualizer);
done:
	return result;
}


enum vv_result gles_visualizer_destroy(struct vv_visualizer* visualizer)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* gles_visualizer = visualizer->derivative;

	glDeleteProgram(gles_visualizer->shader_program);
	glDeleteShader(gles_visualizer->fragment_shader);
	glDeleteShader(gles_visualizer->vertex_shader);

	/* Free these resource if they were allocated by visualizer internally */
	// if(gles_visualizer->texture)
	// 	vv_memory_destroy(&gles_visualizer->texture);

	// if(gles_visualizer->colormap)
	// 	vv_memory_destroy(&gles_visualizer->colormap);

	return result;
}


enum vv_result gles_visualizer_set_viewport(struct vv_visualizer* visualizer, const int width, const int height)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* gles_visualizer = visualizer->derivative;

	if(visualizer->framebuffer)
		vv_memory_destroy(&visualizer->framebuffer);

	result = vv_memory_create
	(
		&visualizer->framebuffer,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = visualizer->context,
			.width = width,
			.height = height,
			.bytes_per_channel = 4,
		},
		NULL
	);


	glBindFramebuffer(GL_FRAMEBUFFER, gles_visualizer->fbo);
#ifdef CONFIG_GLES20_COMPATIBLE
	gles_visualizer->context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLuint64)(visualizer->framebuffer->data), 0);
#else
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLuint64)(visualizer->framebuffer->data), 0);
#endif
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return result;
}


enum vv_result gles_visualizer_set_volume(struct vv_visualizer* visualizer, struct vv_memory* volume)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* gles_visualizer = visualizer->derivative;

	if(gles_visualizer->texture)
		vv_memory_destroy(&gles_visualizer->texture);

	goto_cleanup_if(volume->desc.type != VV_MEMORY_TYPE_GLES_TEXTURE, VV_OPERATION_NOT_SUPPORTED, done);

	if(volume->desc.type != VV_MEMORY_TYPE_GLES_TEXTURE)
	{
		/* copy */
	}else
	{
		gles_visualizer->texture = volume;
	}

done:
	return result;
}


enum vv_result gles_visualizer_set_colormap(struct vv_visualizer* visualizer, struct vv_memory* colormap)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* gles_visualizer = visualizer->derivative;

	if(gles_visualizer->colormap)
		vv_memory_destroy(&gles_visualizer->colormap);

	goto_cleanup_if(colormap->desc.type != VV_MEMORY_TYPE_GLES_TEXTURE, VV_OPERATION_NOT_SUPPORTED, done);

	if(colormap->desc.type != VV_MEMORY_TYPE_GLES_TEXTURE)
	{
		/* copy */
	}else
	{
		gles_visualizer->colormap = colormap;
	}

done:
	return result;

}


static int get_slicing_direction(struct vv_visualizer* visualizer)
{
	float modelview[16];
	float vec[4];
	float depth[6];
	int direction, i, min;

	mat4_load_idendity(modelview);
	mat4_mul(modelview, modelview, visualizer->world);
	mat4_mul(modelview, modelview, visualizer->view);


	/* -Z */

	vec4_set(vec, 0.0, 0.0, -0.5, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[0] = vec[2] / vec[3];


	/* +Z */

	vec4_set(vec, 0.0, 0.0, 0.5, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[1] = vec[2] / vec[3];


	/* -Y */

	vec4_set(vec, 0.0, -0.5, 0.0, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[2] = vec[2] / vec[3];


	/* +Y */

	vec4_set(vec, 0.0, 0.5, 0.0, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[3] = vec[2] / vec[3];


	/* -X */

	vec4_set(vec, -0.5, 0.0, 0.0, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[4] = vec[2] / vec[3];


	/* +X */

	vec4_set(vec, 0.5, 0.0, 0.0, 1.0);
	mat4_mul_vec4(vec, modelview, vec);
	depth[5] = vec[2] / vec[3];

	min = depth[0];
	direction = 0;

	for(i = 0;i < 6;++i)
	{
		if(depth[i] < min)
		{
			min = depth[i];
			direction = i;
		}
	}

	return direction;
}

enum vv_result gles_visualizer_render(struct vv_visualizer* visualizer)
{
	int direction = 0;
	int nr_slices = 0;
	float local[16] = {0.0};
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* gles_visualizer = visualizer->derivative;

	eglMakeCurrent(gles_visualizer->context->display, EGL_NO_SURFACE, EGL_NO_SURFACE, gles_visualizer->context->context);
	glBindFramebuffer(GL_FRAMEBUFFER, gles_visualizer->fbo);
	glUseProgram(gles_visualizer->shader_program);
	glBindVertexArray(gles_visualizer->vao);

	glActiveTexture(GL_TEXTURE0);
	if(gles_visualizer->texture)
		glBindTexture(GL_TEXTURE_3D, (GLuint64)gles_visualizer->texture->data);

	glActiveTexture(GL_TEXTURE1);
	if(gles_visualizer->colormap)
		glBindTexture(GL_TEXTURE_3D, (GLuint64)gles_visualizer->colormap->data);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* Update uniforms */

	glUniformMatrix4fv(gles_visualizer->world_loc, 1, GL_FALSE, visualizer->world);
	glUniformMatrix4fv(gles_visualizer->view_loc, 1, GL_FALSE, visualizer->view);
	glUniformMatrix4fv(gles_visualizer->projection_loc, 1, GL_FALSE, visualizer->projection);

	direction = get_slicing_direction(visualizer);

	local[15] = 1.0;

	switch(direction)
	{
	case 0:
		local[0] = 1.0;
		local[5] = 1.0;
		local[10] = 1.0;
		nr_slices = visualizer->volume->desc.depth;
		break;
	case 1:
		local[0] = 1.0;
		local[5] = 1.0;
		local[10] = -1.0;
		nr_slices = visualizer->volume->desc.depth;
		break;
	case 2:
		local[8] = 1.0;
		local[1] = 1.0;
		local[6] = 1.0;
		nr_slices = visualizer->volume->desc.height;
		break;
	case 3:
		local[8] = 1.0;
		local[1] = 1.0;
		local[6] = -1.0;
		nr_slices = visualizer->volume->desc.height;
		break;
	case 4:
		local[4] = 1.0;
		local[9] = 1.0;
		local[2] = 1.0;
		nr_slices = visualizer->volume->desc.width;
		break;
	case 5:
		local[4] = 1.0;
		local[9] = 1.0;
		local[2] = -1.0;
		nr_slices = visualizer->volume->desc.width;
		break;
	}

	glUniform1i(gles_visualizer->nr_slices_loc, nr_slices);
	glUniformMatrix4fv(gles_visualizer->local_loc, 1, GL_FALSE, local);

	/* Call glDrawArrays */
	glDrawArrays(GL_TRIANGLES, 0, 6 * nr_slices);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	eglMakeCurrent(gles_visualizer->context->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	return result;
}


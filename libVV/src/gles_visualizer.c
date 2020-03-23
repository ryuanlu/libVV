#include <stdlib.h>
#include <GLES3/gl32.h>
#include "gles.h"
#include "context.h"
#include "memory.h"
#include "visualizer.h"
#include "debug.h"

struct gles_visualizer
{
	struct gles_context*	context;
	struct vv_memory*	texture;
	struct vv_memory*	colormap;

	GLuint	vertex_shader;
	GLuint	fragment_shader;
	GLuint	shader_program;

};


enum vv_result gles_visualizer_create(struct vv_visualizer* visualizer)
{
	enum vv_result result = VV_SUCCESS;
	struct gles_visualizer* new_visualizer = NULL;
	char* vert_src = NULL;
	char* frag_src = NULL;

	new_visualizer = calloc(1, sizeof(struct gles_visualizer));
	new_visualizer->context = visualizer->context->gles;

	switch(visualizer->type)
	{
	case VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED:
		vert_src = NULL;
		frag_src = NULL;
		break;
	case VV_VISUALIZER_TYPE_3D_TEXTURE_VIEW_ALIGNED:
		vert_src = NULL;
		frag_src = NULL;
		break;
	default:
		break;
	}

	goto_cleanup_if_failed(gles_create_shader(new_visualizer->context, &new_visualizer->vertex_shader, VV_VERTEX_SHADER, vert_src, -1), new_visualizer_cleanup);
	goto_cleanup_if_failed(gles_create_shader(new_visualizer->context, &new_visualizer->fragment_shader, VV_FRAGMENT_SHADER, frag_src, -1), vertex_shader_cleanup);
	goto_cleanup_if_failed(gles_create_program(new_visualizer->context, &new_visualizer->shader_program, new_visualizer->vertex_shader, new_visualizer->fragment_shader), fragment_shader_cleanup);

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

	if(gles_visualizer->texture)
		vv_memory_destroy(&gles_visualizer->texture);

	if(gles_visualizer->colormap)
		vv_memory_destroy(&gles_visualizer->colormap);

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
	}

done:
	return result;

}


enum vv_result gles_visualizer_render(struct vv_visualizer* visualizer)
{
	enum vv_result result = VV_SUCCESS;
done:
	return result;
}


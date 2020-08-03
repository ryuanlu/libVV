#include <stdlib.h>
#include "visualizer.h"
#include "gles_visualizer.h"
#include "matrix.h"
#include "debug.h"


static PFN_vv_visualizer_create vv_visualizer_create_table[] = 
{
	gles_visualizer_create,
	gles_visualizer_create,
};


static PFN_vv_visualizer_destroy vv_visualizer_destroy_table[] =
{
	gles_visualizer_destroy,
	gles_visualizer_destroy,
};


static PFN_vv_visualizer_set_viewport vv_visualizer_set_viewport_table[] =
{
	gles_visualizer_set_viewport,
	gles_visualizer_set_viewport,
};


static PFN_vv_visualizer_set_volume vv_visualizer_set_volume_table[] =
{
	gles_visualizer_set_volume,
	gles_visualizer_set_volume,
};


static PFN_vv_visualizer_set_colormap vv_visualizer_set_colormap_table[] =
{
	gles_visualizer_set_colormap,
	gles_visualizer_set_colormap,
};


static PFN_vv_visualizer_render vv_visualizer_render_table[] =
{
	gles_visualizer_render,
	gles_visualizer_render,
};


static PFN_vv_visualizer_get_pixels vv_visualizer_get_pixels_table[] =
{
	gles_visualizer_get_pixels,
	gles_visualizer_get_pixels,
};


enum vv_result vv_visualizer_create(struct vv_context* context, struct vv_visualizer** visualizer, const enum vv_visualizer_type type)
{
	enum vv_result result = VV_SUCCESS;
	struct vv_visualizer* new_visualizer = NULL;

	goto_cleanup_if(!context || !visualizer, VV_INVALID_VALUE, done);
	goto_cleanup_if(type < 0 || type >= NUMBER_OF_VV_VISUALIZER_TYPES, VV_INVALID_VALUE, done);

	new_visualizer = calloc(1, sizeof(struct vv_visualizer));

	new_visualizer->type = type;
	new_visualizer->context = context;

	mat4_load_idendity(new_visualizer->world);
	mat4_load_idendity(new_visualizer->view);
	mat4_load_idendity(new_visualizer->projection);

	if(vv_visualizer_create_table[new_visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_create_table[type](new_visualizer), new_visualizer_cleanup);

	*visualizer = new_visualizer;
	goto done;

new_visualizer_cleanup:
	free(new_visualizer);
done:
	return result;
}


enum vv_result vv_visualizer_destroy(struct vv_visualizer** visualizer)
{
	enum vv_result result = VV_SUCCESS;
	goto_cleanup_if(!visualizer, VV_INVALID_VALUE, done);

	if(vv_visualizer_destroy_table[(*visualizer)->type])
		goto_cleanup_if_failed(vv_visualizer_destroy_table[(*visualizer)->type](*visualizer), done);

	*visualizer = NULL;
done:
	return result;
}


enum vv_result vv_visualizer_set_viewport(struct vv_visualizer* visualizer, const int width, const int height)
{
	enum vv_result result = VV_SUCCESS;

	if(vv_visualizer_set_viewport_table[visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_set_viewport_table[visualizer->type](visualizer, width, height), done);

done:
	return result;
}


enum vv_result vv_visualizer_set_volume(struct vv_visualizer* visualizer, struct vv_memory* volume)
{
	enum vv_result result = VV_SUCCESS;

	visualizer->volume = volume;

	if(vv_visualizer_set_volume_table[visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_set_volume_table[visualizer->type](visualizer, volume), done);

done:
	return result;
}


enum vv_result vv_visualizer_set_colormap(struct vv_visualizer* visualizer, struct vv_memory* colormap)
{
	enum vv_result result = VV_SUCCESS;

	visualizer->colormap = colormap;

	if(vv_visualizer_set_colormap_table[visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_set_colormap_table[visualizer->type](visualizer, colormap), done);

done:
	return result;
}


enum vv_result vv_visualizer_render(struct vv_visualizer* visualizer)
{
	enum vv_result result = VV_SUCCESS;

	if(vv_visualizer_render_table[visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_render_table[visualizer->type](visualizer), done);

done:
	return result;
}


enum vv_result vv_visualizer_get_pixels(struct vv_visualizer* visualizer, char* pixels)
{
	enum vv_result result = VV_SUCCESS;

	if(vv_visualizer_get_pixels_table[visualizer->type])
		goto_cleanup_if_failed(vv_visualizer_get_pixels_table[visualizer->type](visualizer, pixels), done);

done:
	return result;
}


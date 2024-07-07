#include <stdlib.h>
#include "volume.h"
#include "volume_texture.h"
#include "isosurface_renderer.h"
#include "gbm.h"
#include "bmp.h"

#define ISO_VALUE	(700)
#define FB_WIDTH	(800)
#define FB_HEIGHT	(800)

int main(int argc, char** argv)
{
	struct volume* volume = NULL;
	struct raw_params params;

	struct gbm_context* context = NULL;
	struct gbm_fb* fb = NULL;
	struct isosurface_renderer* renderer = NULL;
	struct volume_texture* volume_texture = NULL;

	params.width = 256;
	params.height = 256;
	params.depth = 113;
	params.widthscale = 1.0f;
	params.heightscale = 1.0f;
	params.depthscale = 2.0f;
	params.voxelformat = VOXEL_FORMAT_UNSIGNED_16_BE;
	params.bitmask = 16;

	volume = volume_open("../../data/cthead.bin", VOLUME_FILE_TYPE_RAW, &params);

	context = gbm_context_create("/dev/dri/renderD128");
	fb = gbm_fb_create(context, FB_WIDTH, FB_HEIGHT);
	gbm_make_current(context, fb);

	renderer = isosurface_renderer_create();
	volume_texture = volume_texture_create(volume);

	renderer_attach_volume(&renderer->renderer, volume_texture);
	isosurface_renderer_set_isovalue(renderer, ISO_VALUE);

	renderer_resize(&renderer->renderer, FB_WIDTH, FB_HEIGHT);
	renderer_redraw(&renderer->renderer);

	char *ptr = gbm_fb_read_pixels(fb);
	write_pixels_to_bmp("isosurface.bmp", FB_WIDTH, FB_HEIGHT, ptr);

	volume_texture_destroy(volume_texture);
	volume_destroy(volume);
	isosurface_renderer_destroy(renderer);
	gbm_fb_destroy(context, fb);
	gbm_context_destroy(context);


	return 0;
}

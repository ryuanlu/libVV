#include <stdlib.h>
#include "volume.h"
#include "volume_texture.h"
#include "volume_renderer.h"
#include "colormap.h"
#include "gbm.h"
#include "bmp.h"

#define COLORMAP_LEVEL	(64)
#define COLORMAP_HEIGHT	(32)

#define FB_WIDTH	(800)
#define FB_HEIGHT	(800)

int main(int argc, char** argv)
{
	struct volume* volume = NULL;
	struct raw_params params;
	unsigned char* colormap = NULL;
	struct volume_texture* volume_texture = NULL;
	struct volume_renderer* volume_renderer = NULL;
	struct gbm_context* context = NULL;
	struct gbm_fb* fb = NULL;

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

	volume_texture = volume_texture_create(volume);
	volume_renderer = volume_renderer_create();

	colormap = calloc(1, 4 * COLORMAP_LEVEL * COLORMAP_HEIGHT);
	colormap_gen_hsv(colormap, COLORMAP_LEVEL, COLORMAP_HEIGHT);
	volume_renderer_set_colormap(volume_renderer, colormap, COLORMAP_LEVEL, COLORMAP_HEIGHT);
	free(colormap);
	renderer_attach_volume(&volume_renderer->renderer, volume_texture);
	volume_renderer_set_auto_opacity(volume_renderer, 1);
	renderer_resize(&volume_renderer->renderer, FB_WIDTH, FB_HEIGHT);
	renderer_redraw(&volume_renderer->renderer);

	char * ptr = gbm_fb_read_pixels(fb);
	write_pixels_to_bmp("volume_rendering.bmp", FB_WIDTH, FB_HEIGHT, ptr);

	volume_renderer_destroy(volume_renderer);
	volume_texture_destroy(volume_texture);
	volume_destroy(volume);
	gbm_fb_destroy(context, fb);
	gbm_context_destroy(context);

	return 0;
}

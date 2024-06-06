#include <unistd.h>
#include "wayland.h"
#include "isosurface.h"
#include "isosurface_renderer.h"
#include "vertex_buffer.h"
#include "viewer.h"
#include "volume_texture.h"
#include "vv.h"

int run_iso_surface_extraction(struct volume* volume, const struct vv_options* options, const int isovalue)
{
	struct wl_client* wl = NULL;
	struct isosurface_renderer* renderer = NULL;
	struct viewer* viewer = NULL;
	struct volume_texture* volume_texture = NULL;


	wl = wl_client_create();
	viewer = viewer_create(wl, "Iso-surface");
	wl_window_make_current(viewer->window);
	renderer = isosurface_renderer_create();
	viewer_set_renderer(viewer, &renderer->renderer);

	volume_texture = volume_texture_create(volume);
	renderer_attach_volume(&renderer->renderer, volume_texture);
	isosurface_renderer_set_isovalue(renderer, isovalue);

	while(!viewer->quit)
	{
		int r = 0;

		if(wl_client_read_events(wl) > 0)
			r = wl_client_dispatch(wl);

		if(!r)
			usleep(2000);
	}

	volume_texture_destroy(volume_texture);
	viewer_destroy(viewer);
	isosurface_renderer_destroy(renderer);
	wl_client_destroy(wl);

	return 0;
}

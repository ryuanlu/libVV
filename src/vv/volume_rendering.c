#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <linux/input-event-codes.h>
#include <wayland-client.h>
#include <threads.h>
#include "volume.h"
#include "gles.h"
#include "volume_texture.h"
#include "volume_renderer.h"
#include "wayland.h"
#include "matrix.h"
#include "viewer.h"
#include "colormap.h"
#include "tty.h"
#include "panel.h"
#include "vv.h"
#include "volume_rendering.h"

#define COLORMAP_LEVEL	(64)
#define COLORMAP_HEIGHT	(32)
#define container_of(ptr, type, member) (type *)((char *)(ptr) - (char *) &((type *)0)->member)

static int colormap_changed(struct panel* panel, const unsigned char* colormap, const int size, void* userdata)
{
	struct viewer* context = (struct viewer*)userdata;
	struct volume_renderer* volume_renderer = container_of(context->renderer, struct volume_renderer, renderer);
	volume_renderer_set_colormap(volume_renderer, colormap, size, COLORMAP_HEIGHT);
	viewer_redraw(context);
	return 0;
}

static int viewer_keyboard(struct wl_window* window, const int key, const int state, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;
	struct volume_renderer* volume_renderer = container_of(viewer->renderer, struct volume_renderer, renderer);

	static int lighting = 0;
	static int auto_opacity = 0;

	if(key == KEY_L && state == WL_KEYBOARD_KEY_STATE_PRESSED)
	{
		lighting = !lighting;
		volume_renderer_set_lighting(volume_renderer, lighting);
		viewer_redraw(viewer);
	}

	if(key == KEY_O && state == WL_KEYBOARD_KEY_STATE_PRESSED)
	{
		auto_opacity = !auto_opacity;
		volume_renderer_set_auto_opacity(volume_renderer, auto_opacity);
		viewer_redraw(viewer);
	}

	return 0;
}

static int tty_keyboard(struct panel* panel, struct tty_device* tty, const int key, const int modifier, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;

	if(key == 'q')
		viewer->quit = 1;

	return 0;
}

int run_volume_rendering(struct volume* volume, const struct vv_options* options)
{
	struct wl_client* wl = NULL;
	struct viewer* viewer = NULL;
	struct tty_device* tty = NULL;
	struct volume_texture* volume_texture = NULL;
	struct volume_renderer* volume_renderer = NULL;
	unsigned char* colormap = NULL;

	wl = wl_client_create();
	viewer = viewer_create(wl, "Volume Rendering");
	viewer_set_keyboard_handler(viewer, viewer_keyboard);

	wl_window_make_current(viewer->window);
	volume_renderer = volume_renderer_create();
	viewer_set_renderer(viewer, &volume_renderer->renderer);

	colormap = calloc(1, 4 * COLORMAP_LEVEL * COLORMAP_HEIGHT);
	colormap_gen_hsv(colormap, COLORMAP_LEVEL, COLORMAP_HEIGHT);
	volume_renderer_set_colormap(volume_renderer, colormap, COLORMAP_LEVEL, COLORMAP_HEIGHT);

	volume_texture = volume_texture_create(volume);
	renderer_attach_volume(&volume_renderer->renderer, volume_texture);

#define HISTOGRAM_LEVEL (64)
	float histogram[HISTOGRAM_LEVEL];
	volume_gen_histogram(volume, histogram, NULL, HISTOGRAM_LEVEL);


	struct panel* panel = panel_create();

	tty = tty_device_open(NULL);
	tty_device_set_title(tty, "Volume Visualizer");
	tty_device_set_root_widget(tty, &panel->widget);
	panel_set_filename(panel, options->input_filename);
	panel_set_max(panel, volume->maxvalue);
	panel_set_histogram(panel, histogram);
	panel_set_colormap(panel, colormap);
	panel_set_userdata(panel, viewer);
	panel_set_colormap_changed_handler(panel, colormap_changed);
	panel_set_keyboard_handler(panel, tty_keyboard);
	tty_device_redraw(tty);

	free(colormap);

	while(!viewer->quit)
	{
		int r = 0;

		if(wl_client_read_events(wl) > 0)
			r = wl_client_dispatch(wl);

		tty_device_flush(tty);
		tty_device_dispatch(tty);


		if(!r)
			thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 2000000, }, NULL);
	}

	panel_destroy(panel);
	tty_device_close(tty);

	volume_renderer_destroy(volume_renderer);
	volume_texture_destroy(volume_texture);
	viewer_destroy(viewer);
	wl_client_destroy(wl);

	return 0;
}

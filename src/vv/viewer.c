#include <stdlib.h>
#include <linux/input-event-codes.h>
#include <wayland-client.h>
#include "matrix.h"
#include "renderer.h"
#include "wayland.h"
#include "viewer.h"

static int redraw(struct wl_window* window, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;
	float* world;

	world = renderer_get_world_matrix(viewer->renderer);
	mat4_set_translate(world, 0.0f, 0.0f, -viewer->distance);
	mat4_multiplied_by_mat4(world, world, viewer->rotation);
	renderer_redraw(viewer->renderer);
	wl_window_swapbuffers(window);

	return 0;
}

static int mouse_button(struct wl_window* window, const int x, const int y, const int button, const int state, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;

	if(button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
	{
		if(viewer->resize)
			wl_window_resize(window);

		viewer->drag = viewer->resize ? 0 : 1;
		viewer->prev_x = x;
		viewer->prev_y = y;
	}

	if(button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_RELEASED)
	{
		viewer->drag = 0;
	}

	return 0;
}

static int mouse_motion(struct wl_window* window, const int x, const int y, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;
	float rotation[16];

	if(!viewer->drag)
		return 0;

	if(y - viewer->prev_y == 0.0f && x - viewer->prev_x == 0.0f)
		return 0;

	mat4_set_rotation(rotation, y - viewer->prev_y, x - viewer->prev_x, 0.0f, M_PI / 60.0f);
	mat4_multiplied_by_mat4(viewer->rotation, rotation, viewer->rotation);
	redraw(window, userdata);
	viewer->prev_x = x;
	viewer->prev_y = y;

	return 0;
}

static int mouse_wheel(struct wl_window* window, const int value, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;
	viewer->distance += value > 0 ? 0.1f : -0.1f;
	redraw(window, userdata);
	return 0;
}

static int keyboard(struct wl_window* window, const int key, const int state, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;

	if(key == KEY_Q && state == WL_KEYBOARD_KEY_STATE_PRESSED)
		viewer->quit = 1;

	if(key == KEY_LEFTCTRL)
		viewer->resize = state;

	static float ambient = 0.0f;

	if(key == KEY_PAGEUP && state == WL_KEYBOARD_KEY_STATE_PRESSED)
	{
		ambient += 0.1f;
		renderer_set_ambient(viewer->renderer, ambient);
		redraw(viewer->window, viewer);
	}

	if(key == KEY_PAGEDOWN && state == WL_KEYBOARD_KEY_STATE_PRESSED)
	{
		ambient -= 0.1f;
		renderer_set_ambient(viewer->renderer, ambient);
		redraw(viewer->window, viewer);
	}

	if(viewer->keyboard_handler)
		viewer->keyboard_handler(window, key, state, userdata);

	return 0;
}

static int resize(struct wl_window* window, const int width, const int height, void* userdata)
{
	struct viewer* viewer = (struct viewer*)userdata;
	renderer_resize(viewer->renderer, width, height);
	return 0;
}


struct viewer* viewer_create(struct wl_client* wl, const char* title)
{
	struct viewer* viewer = NULL;

	viewer = calloc(1, sizeof(struct viewer));

	viewer->window = wl_window_create(wl, 800, 800);
	viewer->distance = 1.0f;

	wl_window_set_redraw_handler(viewer->window, redraw);
	wl_window_set_resize_handler(viewer->window, resize);
	wl_window_set_pointer_button_handler(viewer->window, mouse_button);
	wl_window_set_pointer_motion_handler(viewer->window, mouse_motion);
	wl_window_set_pointer_wheel_handler(viewer->window, mouse_wheel);
	wl_window_set_keyboard_handler(viewer->window, keyboard);

	wl_window_make_current(viewer->window);

	mat4_set_identity(viewer->rotation);
	wl_window_set_userdata(viewer->window, viewer);
	wl_window_set_title(viewer->window, title);

	return viewer;
}

void viewer_set_renderer(struct viewer* viewer, struct renderer* renderer)
{
	viewer->renderer = renderer;
	renderer_resize(viewer->renderer, 800, 800);
	renderer_set_background_color(viewer->renderer, 1.0f, 1.0f, 1.0f, 1.0f);
}

void viewer_redraw(struct viewer* viewer)
{
	redraw(viewer->window, viewer);
}

void viewer_destroy(struct viewer* viewer)
{
	wl_window_destroy(viewer->window);
	free(viewer);
}

void viewer_set_keyboard_handler(struct viewer* viewer, const PFN_KEYBOARD handler)
{
	viewer->keyboard_handler = handler;
}

#ifndef __VIEWER_H__
#define __VIEWER_H__


#include "wayland.h"

struct renderer;

struct viewer
{
	struct renderer*	renderer;

	int	quit;
	int	drag;
	int	resize;
	int	init;
	int	prev_x;
	int	prev_y;
	float	rotation[16];
	float	distance;
	float	slicing_ratio;

	struct wl_window* window;

	PFN_KEYBOARD	keyboard_handler;
};

struct viewer* viewer_create(struct wl_client* wl, const char* title);
void viewer_set_renderer(struct viewer* viewer, struct renderer* renderer);
void viewer_redraw(struct viewer* viewer);
void viewer_destroy(struct viewer* viewer);
void viewer_set_keyboard_handler(struct viewer* viewer, const PFN_KEYBOARD handler);


#endif /* __VIEWER_H__ */

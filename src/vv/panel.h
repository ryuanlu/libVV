#ifndef __PANEL_H__
#define __PANEL_H__

#include <linux/limits.h>

struct widget;
struct panel;
struct tty_device;

enum mouse_button
{
	MOUSE_BUTTON_NONE,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	NUMBER_OF_MOUSE_BUTTONS,
};

enum modifier
{
	MODIFIER_NONE = 0,
	MODIFIER_SHIFT = 1 << 1,
	MODIFIER_ALT = 1 << 2,
	MODIFIER_CTRL = 1 << 3,
};



typedef int (*PFN_WIDGET_REDRAW)(struct widget* widget, struct tty_device* tty);
typedef int (*PFN_WIDGET_MOUSE_BUTTON)(struct widget* widget, struct tty_device* tty, const int x, const int y, const int button, const int pressed, const int modifier);
typedef int (*PFN_WIDGET_MOUSE_MOTION)(struct widget* widget, struct tty_device* tty, const int x, const int y, const int button, const int pressed, const int modifier);
typedef int (*PFN_WIDGET_MOUSE_WHEEL)(struct widget* widget, struct tty_device* tty, const int x, const int y, const int value, const int modifier);

typedef int (*PFN_WIDGET_KEYBOARD)(struct widget* widget, struct tty_device* tty, const int key, const int modifier);


typedef int (*PFN_PANEL_COLORMAP_CHANGED)(struct panel* panel, const unsigned char* colormap, const int size, void* userdata);
typedef int (*PFN_PANEL_KEYBOARD)(struct panel* panel, struct tty_device* tty, const int key, const int modifier, void* userdata);

struct widget
{

	int	width;
	int	height;
	int	x, y;

	PFN_WIDGET_REDRAW	redraw;
	PFN_WIDGET_MOUSE_BUTTON	mouse_button;
	PFN_WIDGET_MOUSE_MOTION	mouse_motion;
	PFN_WIDGET_MOUSE_WHEEL	mouse_wheel;
	PFN_WIDGET_KEYBOARD	keyboard;
};

struct panel
{
	float*		histogram;
	unsigned char*	colormap;
	char		filename[PATH_MAX];

	int		highlight;
	int		select_mode;
	int		select_from;
	int		select_to;
	struct widget	widget;

	void*				userdata;
	PFN_PANEL_COLORMAP_CHANGED	changed;
	PFN_PANEL_KEYBOARD		keyboard;

};


struct panel* panel_create(void);
void panel_destroy(struct panel* panel);
void panel_set_histogram(struct panel* panel, const float* histogram);
void panel_set_colormap(struct panel* panel, const unsigned char* colormap);
void panel_set_filename(struct panel* panel, const char* filename);
void panel_set_colormap_changed_handler(struct panel* panel, PFN_PANEL_COLORMAP_CHANGED handler);
void panel_set_keyboard_handler(struct panel* panel, PFN_PANEL_KEYBOARD handler);
void panel_set_userdata(struct panel* panel, void* userdata);

#endif /* __PANEL_H__ */

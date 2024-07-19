#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tty.h"
#include "panel.h"


#define container_of(ptr, type, member) (type *)((char *)(ptr) - (char *) &((type *)0)->member)

#define HISTOGRAM_LEVEL		(64)
#define HISTOGRAM_HEIGHT	(16)


static const char* get_block(const int level)
{
	static const char* blocks[] =
	{
		" ",
		"▁",
		"▂",
		"▃",
		"▄",
		"▅",
		"▆",
		"▇",
		"█"
	};

	return blocks[level];
}


static int panel_redraw(struct widget* widget, struct tty_device* tty)
{
	char buf[PATH_MAX * 2];

	struct panel* panel = container_of(widget, struct panel, widget);

	buf[0] = 0;

	tty_device_set_bg_color(tty, 53, 53, 53);
	tty_device_set_fg_color(tty, 255, 255, 255);

	tty_device_set_cursor_position(tty, 2, 2);

	snprintf(buf, sizeof(buf), "🖻 %s", panel->filename);

	tty_device_write(tty, buf);

	tty_device_set_fg_color(tty, 27, 27, 27);
	tty_device_set_cursor_position(tty, 8, 4);
	tty_device_write(tty, "╭");
	tty_device_write_n(tty, "─", 64);
	tty_device_write(tty, "╮");

	for(int i = 0;i < 18;++i)
	{
		tty_device_set_cursor_position(tty, 8, 5 + i);
		tty_device_write(tty, "│");
		tty_device_set_cursor_position(tty, 73, 5 + i);
		tty_device_write(tty, "│");
	}

	tty_device_set_cursor_position(tty, 8, 23);
	tty_device_write(tty, "╰");
	tty_device_write_n(tty, "─", HISTOGRAM_LEVEL);
	tty_device_write(tty, "╯");



	tty_device_set_fg_color(tty, 255, 255, 255);
	tty_device_set_cursor_position(tty, 11, 4);
	tty_device_write(tty, "Histogram");

	tty_device_set_cursor_position(tty, 9, 21);

	for(int i = 0;i < HISTOGRAM_LEVEL;++i)
	{
		tty_device_set_fg_color(tty, panel->colormap[i * 4 + 0], panel->colormap[i * 4 + 1], panel->colormap[i * 4 + 2]);
		tty_device_write(tty, "█");
	}

	tty_device_set_cursor_position(tty, 9, 22);

	for(int i = 0;i < HISTOGRAM_LEVEL;++i)
	{
		tty_device_set_fg_color(tty, panel->colormap[i * 4 + 3], panel->colormap[i * 4 + 3], panel->colormap[i * 4 + 3]);
		tty_device_write(tty, "█");
	}


	tty_device_set_cursor_position(tty, 9, 20);

	for(int i = 0;i < HISTOGRAM_LEVEL;++i)
	{
		int len = HISTOGRAM_HEIGHT * 8 * panel->histogram[i] / 8;
		int extra = (int)(HISTOGRAM_HEIGHT * 8 * panel->histogram[i]) % 8;

		if(panel->highlight == i)
		{
			tty_device_set_fg_color(tty, 42, 166, 255);
			tty_device_set_bg_color(tty, 63, 63, 63);
		}else
		{
			tty_device_set_fg_color(tty, 21, 83, 158);
			tty_device_set_bg_color(tty, 45, 45, 45);
		}

		int left = panel->select_from > panel->select_to ? panel->select_to : panel->select_from;
		int right = panel->select_from > panel->select_to ? panel->select_from : panel->select_to;

		if(i >= left && i <= right)
		{
			tty_device_set_fg_color(tty, 42, 255, 255);
		}


		tty_device_set_cursor_position(tty, 9 + i, 20);

		for(int j = len + !!extra;j < 16;++j)
		{
			tty_device_set_cursor_position(tty, 9 + i, 20 - j);
			tty_device_write(tty, " ");
		}

		tty_device_set_cursor_position(tty, 9 + i, 20);
		
		for(int j = 0;j < len;++j)
		{
			tty_device_set_cursor_position(tty, 9 + i, 20 - j);
			tty_device_write(tty, "█");
		}

		if(extra)
		{
			tty_device_set_cursor_position(tty, 9 + i, 20 - len);
			tty_device_write(tty, get_block(extra));
		}

	}

	return 0;
}


static int panel_mouse_button(struct widget* widget, struct tty_device* tty, const int x, const int y, const int button, const int pressed, const int modifier)
{
	struct panel* panel = container_of(widget, struct panel, widget);

	if(x > 8 && x < 73 && y > 5 && y < 21)
	{
		if(pressed)
		{
			panel->select_from = x - 9;
			panel->select_to = x - 9;
			panel->select_mode = 1;
		}
		else
		{
			panel->select_mode = 0;
		}
		panel_redraw(widget, tty);
	}

	return 0;
}

static int panel_mouse_motion(struct widget* widget, struct tty_device* tty, const int x, const int y, const int button, const int pressed, const int modifier)
{
	struct panel* panel = container_of(widget, struct panel, widget);

	if(x > 8 && x < 73 && y > 5 && y < 21)
	{
		panel->highlight = x - 9;

		if(panel->select_mode)
			panel->select_to = x - 9;

		panel_redraw(widget, tty);
	}
	else
	{
		panel->highlight = -1;
	}
		
	return 0;
}

static int panel_mouse_wheel(struct widget* widget, struct tty_device* tty, const int x, const int y, const int value, const int modifier)
{
	struct panel* panel = container_of(widget, struct panel, widget);

	int left = panel->select_from > panel->select_to ? panel->select_to : panel->select_from;
	int right = panel->select_from > panel->select_to ? panel->select_from : panel->select_to;

	if(left < 0)
		return 0;

	for(int i = left;i <= right;++i)
	{
		int opacity = panel->colormap[i * 4 + 3] + value * 5;
		opacity = opacity < 0 ? 0 : opacity;
		opacity = opacity > 255 ? 255 : opacity;
		for(int j = 0;j < HISTOGRAM_HEIGHT * 2;++j)
			panel->colormap[j * HISTOGRAM_LEVEL * 4 + i * 4 + 3] = opacity;
	}

	panel_redraw(widget, tty);

	if(panel->changed)
		panel->changed(panel, panel->colormap, HISTOGRAM_LEVEL, panel->userdata);

	return 0;
}


static int panel_keyboard(struct widget* widget, struct tty_device* tty, const int key, const int modifier)
{
	struct panel* panel = container_of(widget, struct panel, widget);

	if(panel->keyboard)
		panel->keyboard(panel, tty, key, modifier, panel->userdata);

	return 0;
}


struct panel* panel_create(void)
{
	struct panel* panel = NULL;

	panel = calloc(1, sizeof(struct panel));
	panel->widget.redraw = panel_redraw;
	panel->widget.mouse_button = panel_mouse_button;
	panel->widget.mouse_motion = panel_mouse_motion;
	panel->widget.mouse_wheel = panel_mouse_wheel;
	panel->widget.keyboard = panel_keyboard;

	panel->histogram = calloc(1, sizeof(float) * HISTOGRAM_LEVEL);
	panel->colormap = calloc(1, HISTOGRAM_LEVEL * HISTOGRAM_HEIGHT * 2 * 4);

	panel->highlight = -1;
	panel->select_mode = 0;
	panel->select_from = -1;
	panel->select_to = -1;

	return panel;
}


void panel_destroy(struct panel* panel)
{
	if(panel->histogram)
		free(panel->histogram);
	if(panel->colormap)
		free(panel->colormap);

	free(panel);
}

void panel_set_histogram(struct panel* panel, const float* histogram)
{
	memcpy(panel->histogram, histogram, sizeof(float) * HISTOGRAM_LEVEL);
}

void panel_set_colormap(struct panel* panel, const unsigned char* colormap)
{
	memcpy(panel->colormap, colormap, 4 * HISTOGRAM_LEVEL * HISTOGRAM_HEIGHT * 2);
}

void panel_set_filename(struct panel* panel, const char* filename)
{
	strncpy(panel->filename, filename, PATH_MAX - 1);
}

void panel_set_colormap_changed_handler(struct panel* panel, PFN_PANEL_COLORMAP_CHANGED handler)
{
	panel->changed = handler;
}

void panel_set_keyboard_handler(struct panel* panel, PFN_PANEL_KEYBOARD handler)
{
	panel->keyboard = handler;
}

void panel_set_userdata(struct panel* panel, void* userdata)
{
	panel->userdata = userdata;
}

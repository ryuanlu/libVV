#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "VV.h"

#define TITLE_STRING	"libVV Test"

#define TEST_DATA_PATH	"../cthead.bin"
#define TEST_DATA_WIDTH		(256)
#define TEST_DATA_HEIGHT	(256)
#define TEST_DATA_DEPTH		(113)
#define TEST_DATA_SIZE		(TEST_DATA_WIDTH * TEST_DATA_HEIGHT * TEST_DATA_DEPTH * 2)

#define COLORMAP_PATH	"../hsv.bin"
#define COLORMAP_WIDTH	(2048)

#define FRAMEBUFFER_WIDTH	(800)
#define FRAMEBUFFER_HEIGHT	(800)


vv_memory* load_texture(const vv_context* context, const char* filename, const int width, const int height, const int depth, const int bpp)
{
	int size = width * height * depth * bpp;
	char* data = NULL;
	vv_memory* texture = NULL;
	FILE* fp = NULL;


	fp = fopen(filename, "r");
	data = calloc(size, 1);
	fread(data, size, 1, fp);
	fclose(fp);

	vv_memory_create
	(
		&texture,
		&(const vv_memory_desc)
		{
			.type = VV_MEMORY_TYPE_GLES_TEXTURE,
			.context = context,
			.width = width,
			.height = height,
			.depth = depth,
			.bytes_per_channel = bpp,
		},
		data
	);

	free(data);

	return texture;
}


int main(int argc, char** argv)
{
	const char	title_string[] = TITLE_STRING;
	int		quit = False;
	Display		*X_display = NULL;
	Window		window;
	XTextProperty	title;
	XEvent		event;
	Atom		wm_delete_window;

	vv_context* context = NULL;
	vv_memory* volume = NULL;
	vv_memory* colormap = NULL;
	vv_visualizer* visualizer = NULL;


	X_display = XOpenDisplay(NULL);
	window = XCreateWindow(X_display, RootWindow(X_display, 0), 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, CopyFromParent, CopyFromParent, CopyFromParent, 0, NULL);
	XSetWindowBackground(X_display, window, BlackPixel(X_display, 0));
	title.value = (unsigned char*)title_string;
	title.encoding = XA_STRING;
	title.format = 8;
	title.nitems = strlen(title_string);
	XSetWMProperties(X_display, window, &title, &title, NULL, 0, NULL, NULL, NULL);
	wm_delete_window = XInternAtom(X_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(X_display, window, &wm_delete_window, 1);
	XSelectInput(X_display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
	XMapWindow(X_display, window);


	vv_context_create(&context);

	volume = load_texture(context, TEST_DATA_PATH, TEST_DATA_WIDTH, TEST_DATA_HEIGHT, TEST_DATA_DEPTH, 2);
	colormap = load_texture(context, COLORMAP_PATH, COLORMAP_WIDTH, 1, 1, 4);

	vv_visualizer_create(context, &visualizer, VV_VISUALIZER_TYPE_3D_TEXTURE_AXIS_ALIGNED);
	vv_visualizer_set_viewport(visualizer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	vv_visualizer_set_volume(visualizer, volume);
	vv_visualizer_set_colormap(visualizer, colormap);
	vv_visualizer_render(visualizer);

	while(!quit)
	{
		if(XPending(X_display))
		{
			XNextEvent(X_display, &event);
			switch(event.type)
			{
			case ClientMessage:
				if(event.xclient.data.l[0] == wm_delete_window)
				{
					XDestroyWindow(X_display, window);
					quit = True;
				}
				break;
			}
		}
	}

	XCloseDisplay(X_display);

	vv_visualizer_destroy(&visualizer);
	vv_memory_destroy(&volume);
	vv_context_destroy(&context);
	return EXIT_SUCCESS;
}

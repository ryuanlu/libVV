#include <stdio.h>
#include <stdlib.h>
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gbm.h>
#include <fcntl.h>
#include <unistd.h>
#include "gles.h"
#include "gbm.h"


struct gbm_context
{
	int	fd;
	struct gbm_device*	device;


	EGLDisplay		egl_display;
	EGLContext		egl_context;
	EGLConfig		egl_config;

};

struct gbm_fb
{
	struct gbm_surface*	gbm_surface;
	EGLSurface		egl_surface;

	int	width;
	int	height;

	char*	pixels;

	struct gbm_bo*	bo;
	uint32_t	stride;
	void*		mapping;

};

struct gbm_context* gbm_context_create(const char* device)
{
	struct gbm_context* context = NULL;

	context = calloc(1, sizeof(struct gbm_context));

	context->fd = open(device, O_RDWR);

	context->device = gbm_create_device(context->fd);

	context->egl_display = eglGetPlatformDisplay(EGL_PLATFORM_GBM_KHR, context->device, NULL);
	eglInitialize(context->egl_display, NULL, NULL);
	eglBindAPI(EGL_OPENGL_ES_API);
	context->egl_config = gles_get_eglconfig(context->egl_display);
	context->egl_context = eglCreateContext(context->egl_display, context->egl_config, EGL_NO_CONTEXT, gles_get_egl_context_attributes());

	return context;
}

void gbm_context_destroy(struct gbm_context* context)
{
	eglMakeCurrent(context->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(context->egl_display, context->egl_context);
	eglTerminate(context->egl_display);
	gbm_device_destroy(context->device);
	close(context->fd);
	free(context);
}

struct gbm_fb* gbm_fb_create(struct gbm_context* context, const int width, const int height)
{
	struct gbm_fb* fb = NULL;

	fb = calloc(1, sizeof(struct gbm_fb));
	fb->gbm_surface = gbm_surface_create(context->device, width, height, GBM_FORMAT_XRGB8888, GBM_BO_USE_RENDERING|GBM_BO_USE_LINEAR);
	fb->egl_surface = eglCreateWindowSurface(context->egl_display, context->egl_config, (EGLNativeWindowType)fb->gbm_surface, NULL);
	fb->width = width;
	fb->height = height;

	return fb;
}

void gbm_fb_destroy(struct gbm_context* context, struct gbm_fb* fb)
{
	if(fb->bo)
		gbm_fb_unmap(fb);
	eglDestroySurface(context->egl_display, fb->egl_surface);
	gbm_surface_destroy(fb->gbm_surface);

	if(fb->pixels)
		free(fb->pixels);

	free(fb);
}

char* gbm_fb_read_pixels(struct gbm_fb* fb)
{
	if(!fb->pixels)
	{
		fb->pixels = calloc(1, 4 * fb->width * fb->height);
	}

	glReadPixels(0, 0, fb->width, fb->height, 0x80E1 /* GL_BGRA */, GL_UNSIGNED_BYTE, fb->pixels);

	return fb->pixels;
}

char* gbm_fb_map(struct gbm_fb* fb)
{
	char* ptr = NULL;

	eglSwapBuffers(eglGetCurrentDisplay(), fb->egl_surface);
	fb->bo = gbm_surface_lock_front_buffer(fb->gbm_surface);
	ptr = gbm_bo_map(fb->bo, 0, 0, fb->width, fb->height, GBM_BO_TRANSFER_READ, &fb->stride, &fb->mapping);

	return ptr;
}

void gbm_fb_unmap(struct gbm_fb* fb)
{
	gbm_bo_unmap(fb->bo, fb->mapping);
	gbm_surface_release_buffer(fb->gbm_surface, fb->bo);

	fb->bo = NULL;
	fb->mapping = NULL;
}

int gbm_make_current(struct gbm_context* context, struct gbm_fb* fb)
{
	if(fb)
	{
		eglMakeCurrent(context->egl_display, fb->egl_surface, fb->egl_surface, context->egl_context);
	}else
	{
		eglMakeCurrent(context->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	}

	return 0;
}

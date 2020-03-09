#include <stdio.h>
#include "cl.h"

#define if_failed(expr, error_code, goto_label) if(!(expr)) { fprintf(stderr, "%s:%d:\t%s() returns %d\n", __FILE__, __LINE__, __FUNCTION__, error_code); result = error_code; goto goto_label; }


int cl_context_create(struct cl_context** context)
{
	int result = 0;
	struct cl_context* new_context = NULL;

	cl_platform_id	platform = NULL;
	cl_device_id	device_id = NULL;

	char version[32] = {0};

	if_failed(context, 1, done);

	new_context = calloc(1, sizeof(struct cl_context));

	clGetPlatformIDs(1, &platform, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(version), version, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	new_context->context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);

	if_failed(new_context->context, 2, new_context_cleanup);

	if(version[8] < '2')
		new_context->queue = clCreateCommandQueue(new_context->context, device_id, 0, NULL);
	else
		new_context->queue = clCreateCommandQueueWithProperties(new_context->context, device_id, 0, NULL);

	if_failed(new_context->queue, 3, clcontext_cleanup);

	*context = new_context;

	goto done;

clcontext_cleanup:
	clReleaseContext(new_context->context);
new_context_cleanup:
	free(new_context);
done:
	return result;
}


int cl_context_destroy(struct cl_context** context)
{
	int result = 0;

	if_failed(context, 1, done);

	clReleaseCommandQueue((*context)->queue);
	clReleaseContext((*context)->context);
	free(*context);
	*context = NULL;

done:
	return result;
}




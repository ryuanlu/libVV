#include <stdio.h>
#include "context.h"
#include "cl.h"
#include "debug.h"


enum vv_result cl_context_create(struct cl_context** context)
{
	enum vv_result result = VV_SUCCESS;
	struct cl_context* new_context = NULL;

	cl_platform_id	platform = NULL;
	cl_device_id	device_id = NULL;

	char version[32] = {0};

	goto_cleanup_if(!context, VV_INVALID_VALUE, done);

	new_context = calloc(1, sizeof(struct cl_context));

	clGetPlatformIDs(1, &platform, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(version), version, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	new_context->context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);

	goto_cleanup_if(!new_context->context, VV_FAILED_TO_INITIALIZE, new_context_cleanup);

	if(version[8] < '2')
		new_context->queue = clCreateCommandQueue(new_context->context, device_id, 0, NULL);
	else
		new_context->queue = clCreateCommandQueueWithProperties(new_context->context, device_id, 0, NULL);

	goto_cleanup_if(!new_context->queue, VV_FAILED_TO_INITIALIZE, clcontext_cleanup);

	*context = new_context;

	goto done;

clcontext_cleanup:
	clReleaseContext(new_context->context);
new_context_cleanup:
	free(new_context);
done:
	return result;
}


enum vv_result cl_context_destroy(struct cl_context** context)
{
	enum vv_result result = VV_SUCCESS;

	goto_cleanup_if(!context, VV_INVALID_VALUE, done);

	clReleaseCommandQueue((*context)->queue);
	clReleaseContext((*context)->context);
	free(*context);
	*context = NULL;

done:
	return result;
}


enum vv_result cl_buffer_create(struct vv_memory* memory, void* extra)
{
	enum vv_result result = VV_SUCCESS;
	cl_int err = 0;

	goto_cleanup_if(!memory || !memory->desc.context, VV_INVALID_CONTEXT, done);

	memory->data = clCreateBuffer(memory->desc.context->cl->context, CL_MEM_READ_WRITE, memory->desc.slice_pitch * memory->desc.bytes_per_channel, NULL, &err);

	goto_cleanup_if(!memory->data, VV_BAD_ALLOCATION, done);

done:
	return result;
}


enum vv_result cl_buffer_destroy(struct vv_memory* memory)
{
	enum vv_result result = VV_SUCCESS;

	goto_cleanup_if(!memory || !memory->desc.context, VV_INVALID_CONTEXT, done);

	clReleaseMemObject(memory->data);
done:
	return result;
}


enum vv_result cl_buffer_map(struct vv_memory* memory, void** ptr)
{
	cl_int err = 0;
	*ptr = clEnqueueMapBuffer(memory->desc.context->cl->queue, memory->data, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, memory->desc.slice_pitch * memory->desc.depth, 0, NULL, NULL, &err);
	return err == CL_SUCCESS ? VV_SUCCESS : VV_OPERATION_NOT_SUPPORTED;
}


enum vv_result cl_buffer_unmap(struct vv_memory* memory, void** ptr)
{
	clEnqueueUnmapMemObject(memory->desc.context->cl->queue, memory->data, *ptr, memory->desc.slice_pitch * memory->desc.depth, NULL, NULL);
	*ptr = NULL;
	return VV_SUCCESS;
}


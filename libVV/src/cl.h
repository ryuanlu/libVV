#ifndef __CL_H__
#define __CL_H__

#define CL_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

struct cl_context
{
	cl_context		context;
	cl_command_queue	queue;
};

int cl_context_create(struct cl_context** context);
int cl_context_destroy(struct cl_context** context);

#endif /* __CL_H__ */
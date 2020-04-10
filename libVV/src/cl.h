#ifndef __CL_H__
#define __CL_H__

#define CL_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include "memory.h"

struct cl_context
{
	cl_context		context;
	cl_command_queue	queue;
};

enum vv_result	cl_context_create	(struct cl_context** context);
enum vv_result	cl_context_destroy	(struct cl_context** context);

enum vv_result	cl_buffer_create	(struct vv_memory* memory, void* extra);
enum vv_result	cl_buffer_destroy	(struct vv_memory* memory);

enum vv_result	cl_buffer_map	(const struct vv_memory* memory, void** ptr);
enum vv_result	cl_buffer_unmap	(const struct vv_memory* memory, void** ptr);

#endif /* __CL_H__ */
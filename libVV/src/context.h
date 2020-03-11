#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "gles.h"
#include "cl.h"

struct vv_context
{
	int	capability;

	struct gles_context*	gles;
	struct cl_context*	cl;
};



#endif /* __CONTEXT_H__ */
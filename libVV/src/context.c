#include <stdlib.h>
#include "VV.h"
#include "context.h"

int vv_context_create(struct vv_context** context)
{
	struct vv_context* new_context = NULL;

	new_context = calloc(sizeof(struct vv_context), 1);
	gles_context_create(&new_context->gles);
	cl_context_create(&new_context->cl);
	*context = new_context;

	return 0;
}


int vv_context_destroy(struct vv_context** context)
{
	cl_context_destroy(&(*context)->cl);
	gles_context_destroy(&(*context)->gles);
	free(*context);
	*context = NULL;

	return 0;
}

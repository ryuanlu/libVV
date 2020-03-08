#include <stdlib.h>
#include "gles.h"
#include "VV.h"


struct vv_context
{
	struct gles_context*	gles;
};


int vv_context_create(struct vv_context** context)
{
	struct vv_context* new_context = NULL;

	new_context = calloc(sizeof(struct vv_context), 1);
	gles_context_create(&new_context->gles);

	*context = new_context;

	return 0;
}


int vv_context_destroy(struct vv_context** context)
{
	gles_context_destroy(&(*context)->gles);
	free(*context);
	*context = NULL;

	return 0;
}

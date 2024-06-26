#ifndef __VV_H__
#define __VV_H__

#include <limits.h>
#include "volume.h"

enum vv_actions
{
	VV_ACTION_VOLUME_RENDERING,
	VV_ACTION_ISO_SURFACE_EXTRACTION,
	NUMBER_OF_VV_ACTIONS,
};

struct vv_options
{
	char			filename[PATH_MAX];
	enum volume_file_type	type;
	struct raw_params	params;
	enum vv_actions		action;
	int			isovalue;
};

#endif /* __VV_H__ */
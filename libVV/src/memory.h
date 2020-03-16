#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "VV.h"

struct vv_memory
{
	struct vv_memory_desc	desc;
	void* data;
};

typedef enum vv_result	(*PFN_vv_memory_create)		(struct vv_memory* memory, void* extra);
typedef enum vv_result	(*PFN_vv_memory_destroy)	(struct vv_memory* memory);

typedef enum vv_result	(*PFN_vv_memory_map)	(struct vv_memory* memory, void** ptr);
typedef enum vv_result	(*PFN_vv_memory_unmap)	(struct vv_memory* memory, void** ptr);

#endif /* __MEMORY_H__ */
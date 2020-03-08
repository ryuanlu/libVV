#ifndef __VV_H__
#define __VV_H__

typedef struct vv_context vv_context;

int vv_context_create(vv_context** context);
int vv_context_destroy(vv_context** context);

#endif /* __VV_H__ */
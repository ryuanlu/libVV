#ifndef __ISO_SURFACE_H__
#define __ISO_SURFACE_H__


struct volume;
struct vertex_buffer;

int isosurface_extract(const struct volume* volume, const int iso_value, struct vertex_buffer* vertex_buffer);



#endif /* __ISO_SURFACE_H__ */
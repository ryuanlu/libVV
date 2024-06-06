#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__


struct vertex_buffer
{
	int	count;
	int	max_count;
	float*	buffer;
};


struct vertex_buffer* vertex_buffer_create(int size);
int vertex_buffer_destroy(struct vertex_buffer* vertex_buffer);
int vertex_buffer_add_triangle(struct vertex_buffer* vertex_buffer, float* triangle);
int vertex_buffer_add_vertex(struct vertex_buffer* vertex_buffer, float* vertex);
int vertex_buffer_clear(struct vertex_buffer* vertex_buffer);


#endif /* __VERTEX_BUFFER_H__ */

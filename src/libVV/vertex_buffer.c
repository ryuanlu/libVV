#include <stdlib.h>
#include <string.h>
#include "vertex_buffer.h"



struct vertex_buffer* vertex_buffer_create(int size)
{
	struct vertex_buffer* vertex_buffer = NULL;

	vertex_buffer = calloc(1, sizeof(struct vertex_buffer));

	vertex_buffer->buffer = calloc(1, sizeof(float) * 3 * size);
	vertex_buffer->max_count = size;

	return vertex_buffer;
}

int vertex_buffer_destroy(struct vertex_buffer* vertex_buffer)
{
	if(vertex_buffer->buffer)
		free(vertex_buffer->buffer);

	free(vertex_buffer);

	return 0;
}


int vertex_buffer_add_triangle(struct vertex_buffer* vertex_buffer, float* triangle)
{
	for(int i = 0;i < 3;++i)
	{
		if(vertex_buffer_add_vertex(vertex_buffer, &triangle[i * 3]))
			return 1;
	}

	return 0;
}


int vertex_buffer_add_vertex(struct vertex_buffer* vertex_buffer, float* vertex)
{
	if(vertex_buffer->max_count == vertex_buffer->count)
	{
		float* ptr = realloc(vertex_buffer->buffer, vertex_buffer->max_count * 2 * sizeof(float) * 3);

		if(ptr)
		{
			vertex_buffer->buffer = ptr;
			vertex_buffer->max_count *= 2;
		}else
		{
			return 1;
		}
	}

	memcpy(&vertex_buffer->buffer[vertex_buffer->count * 3], vertex, sizeof(float) * 3);
	++vertex_buffer->count;

	return 0;
}


int vertex_buffer_clear(struct vertex_buffer* vertex_buffer)
{
	vertex_buffer->count = 0;
	return 0;
}

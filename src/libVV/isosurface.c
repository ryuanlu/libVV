#include <stdio.h>
#include <string.h>
#include <math.h>
#include "isosurface.h"
#include "volume.h"
#include "vertex_buffer.h"

static const int tetrahedron_index_table[40] =
{
	0, 1, 2, 4,
	1, 2, 7, 3,
	2, 4, 7, 6,
	1, 2, 4, 7,
	1, 4, 5, 7,

	4, 0, 6, 5,
	0, 6, 3, 2,
	6, 5, 3, 7,
	0, 6, 5, 3,
	0, 5, 1, 3,
};

static const int egde_indices[12] =
{
	0, 1, 1, 2,
	0, 2, 2, 3,
	0, 3, 3, 1
};

#define get_index(x, y, z)	(volume->params.width * volume->params.height * (z) + volume->params.width * (y) + (x))

static int get_value(const struct volume* volume, const int x, const int y, const int z)
{
	switch (volume->params.voxelformat)
	{
	case VOXEL_FORMAT_UNSIGNED_16_LE:
		return volume->data.u16[get_index(x, y, z)];
	case VOXEL_FORMAT_UNSIGNED_8:
		return volume->data.u8[get_index(x, y, z)];
	default:
		return 0;
	}
}

static int is_dup(float* list, int n, float x, float y, float z)
{
	for(int i = 0;i < n;++i)
	{
		if(list[i * 3] == x && list[i * 3 + 1] == y && list[i * 3 + 2] == z)
			return 1;
	}

	return 0;
}

static void normalize_vertices(const struct volume* volume, int n, float* vertices)
{
	for(int i = 0;i < n;++i)
	{
		vertices[i * 3 + 0] = vertices[i * 3 + 0] / volume->params.width - 0.5f;
		vertices[i * 3 + 1] = vertices[i * 3 + 1] / volume->params.height - 0.5f;
		vertices[i * 3 + 2] = vertices[i * 3 + 2] / volume->params.depth - 0.5f;
	}
}

static void calculate_center(const float* vertices, const int n, float* output)
{
	for(int i = 0;i < 3;++i)
	{
		float sum = 0.0f;

		for(int j = 0;j < n;++j)
			sum += vertices[j * 3 + i];

		output[i] = sum / n;
	}
}

static int marching_tetrahedron(const struct volume* volume, const int x, const int y, const int z, const int iso_value, struct vertex_buffer* vertex_buffer)
{
	int total = 0;
	int mode = (x + y + z) % 2;
	const int *tetrahedron_indices = NULL;
	float vertices[12];

	if(x > (volume->params.width - 2) || y > (volume->params.height - 2) || z > (volume->params.depth - 2))
		return 0;

	memset(vertices, 0, sizeof(vertices));

	for(int tetrahedron = 0;tetrahedron < 5;++tetrahedron)
	{
		int count = 0;
		int tetrahedron_vertices[12];

		tetrahedron_indices = &tetrahedron_index_table[mode * 5 * 4 + tetrahedron * 4];

		for(int vertex = 0;vertex < 4;++vertex)
		{
			int dx, dy, dz;

			dx = !!(tetrahedron_indices[vertex] & 1);
			dy = !!(tetrahedron_indices[vertex] & 2);
			dz = !!(tetrahedron_indices[vertex] & 4);

			tetrahedron_vertices[vertex * 3 + 0] = x + dx;
			tetrahedron_vertices[vertex * 3 + 1] = y + dy;
			tetrahedron_vertices[vertex * 3 + 2] = z + dz;
		}

		for(int edge = 0;edge < 6;++edge)
		{
			float rx, ry, rz;
			int value_a, xa, ya, za;
			int value_b, xb, yb, zb;

			xa = tetrahedron_vertices[3 * egde_indices[edge * 2 + 0] + 0];
			ya = tetrahedron_vertices[3 * egde_indices[edge * 2 + 0] + 1];
			za = tetrahedron_vertices[3 * egde_indices[edge * 2 + 0] + 2];

			xb = tetrahedron_vertices[3 * egde_indices[edge * 2 + 1] + 0];
			yb = tetrahedron_vertices[3 * egde_indices[edge * 2 + 1] + 1];
			zb = tetrahedron_vertices[3 * egde_indices[edge * 2 + 1] + 2];

			value_a = get_value(volume, xa, ya, za);
			value_b = get_value(volume, xb, yb, zb);

			if(value_a == value_b)
				continue;

			if((value_a <= iso_value && iso_value <= value_b) || (value_b <= iso_value && iso_value <= value_a))
			{
				rx = (float)(iso_value - value_a) * (xb - xa) / (value_b - value_a) + xa;
				ry = (float)(iso_value - value_a) * (yb - ya) / (value_b - value_a) + ya;
				rz = (float)(iso_value - value_a) * (zb - za) / (value_b - value_a) + za;

				if(!is_dup(vertices, 4, rx, ry, rz))
				{
					vertices[count * 3 + 0] = rx;
					vertices[count * 3 + 1] = ry;
					vertices[count * 3 + 2] = rz;
					++count;
				}
			}
		}

		if(count < 3 || !vertex_buffer)
			continue;

		normalize_vertices(volume, count, vertices);

		if(count == 3)
		{
			vertex_buffer_add_triangle(vertex_buffer, vertices);
		}
		else
		{
			float center[4];

			calculate_center(vertices, 4, center);

			for(int i = 0;i < 4;++i)
			{
				vertex_buffer_add_vertex(vertex_buffer, center);
				vertex_buffer_add_vertex(vertex_buffer, &vertices[i * 3]);
				vertex_buffer_add_vertex(vertex_buffer, &vertices[((i + 1) % 4) * 3]);
			}
			count = 12;
		}

		total += count;
		count = 0;
	}

	return total;
}

int isosurface_extract(const struct volume* volume, const int iso_value, struct vertex_buffer* vertex_buffer)
{
	for(int i = 0;i < volume->nr_voxels;++i)
	{
		int x, y, z;

		z = i / (volume->params.width * volume->params.height);
		y = (i / volume->params.width) % volume->params.height;
		x = i % volume->params.width;

		marching_tetrahedron(volume, x, y, z, iso_value, vertex_buffer);
	}

	return 0;
}

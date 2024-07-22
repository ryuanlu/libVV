#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "volume.h"
#include "octree.h"


struct octree
{
	int	current;
	int	nr_nodes;
	struct octree_node* nodes;
};

#define get_index(v, x, y, z)	(v->params.width * v->params.height * (z) + v->params.width * (y) + (x))


static int octree_insert(const struct volume* volume, struct octree* octree, const int level, const int begin_x, const int begin_y, const int begin_z, const int end_x, const int end_y, const int end_z)
{
	int index = octree->current;
	struct octree_node* node = &octree->nodes[index];
	++octree->current;

	node->begin.x = begin_x;
	node->begin.y = begin_y;
	node->begin.z = begin_z;
	node->end.x = end_x;
	node->end.y = end_y;
	node->end.z = end_z;

	node->maximum = 0;
	node->minimum = (1 << 16) - 1;

	if(level == 0)
	{
		for(int z = begin_z;z <= end_z;++z)
		{
			for(int y = begin_y;y <= end_y;++y)
			{
				for(int x = begin_x;x <= end_x;++x)
				{
					int value = 0;

					switch(volume->params.voxelformat)
					{
					case VOXEL_FORMAT_UNSIGNED_8:
						value = volume->data.u8[get_index(volume, x, y, z)];
						break;
					case VOXEL_FORMAT_UNSIGNED_16_LE:
						value = volume->data.u16[get_index(volume, x, y, z)];
						break;
					default:
						break;
					}

					if(node->maximum < value)
						node->maximum = value;
					if(node->minimum > value)
						node->minimum = value;
				}
			}
		}

		return index;
	}

	int mid_x, mid_y, mid_z;

	mid_x = (begin_x + end_x) / 2;
	mid_y = (begin_y + end_y) / 2;
	mid_z = (begin_z + end_z) / 2;

	node->children[0] = octree_insert(volume, octree, level - 1, begin_x, begin_y, begin_z, mid_x, mid_y, mid_z);
	node->children[1] = octree_insert(volume, octree, level - 1, mid_x + 1, begin_y, begin_z, end_x, mid_y, mid_z);
	node->children[2] = octree_insert(volume, octree, level - 1, begin_x, mid_y + 1, begin_z, mid_x, end_y, mid_z);
	node->children[3] = octree_insert(volume, octree, level - 1, mid_x + 1, mid_y + 1, begin_z, end_x, end_y, mid_z);
	node->children[4] = octree_insert(volume, octree, level - 1, begin_x, begin_y, mid_z + 1, mid_x, mid_y, end_z);
	node->children[5] = octree_insert(volume, octree, level - 1, mid_x + 1, begin_y, mid_z + 1, end_x, mid_y, end_z);
	node->children[6] = octree_insert(volume, octree, level - 1, begin_x, mid_y + 1, mid_z + 1, mid_x, end_y, end_z);
	node->children[7] = octree_insert(volume, octree, level - 1, mid_x + 1, mid_y + 1, mid_z + 1, end_x, end_y, end_z);

	for(int i = 0;i < 8;++i)
	{
		if(node->maximum < octree->nodes[node->children[i]].maximum)
			node->maximum = octree->nodes[node->children[i]].maximum;

		if(node->minimum > octree->nodes[node->children[i]].minimum)
			node->minimum = octree->nodes[node->children[i]].minimum;
	}

	return index;
}


struct octree_node* octree_create(const struct volume* volume)
{
	int min_level = 10;
	struct octree octree;

	int level = 0;

	level =	log2(volume->params.width);
	min_level = min_level > level ? level : min_level;
	level =	log2(volume->params.height);
	min_level = min_level > level ? level : min_level;
	level =	log2(volume->params.depth);
	min_level = min_level > level ? level : min_level;

	min_level = (min_level - 4) < 0 ? min_level : (min_level - 4);

	octree.current = 0;
	octree.nr_nodes = (1 - (8 << (3 * min_level))) / -7;
	octree.nodes = calloc(1, sizeof(struct octree_node) * octree.nr_nodes);

	octree_insert(volume, &octree, min_level, 0, 0, 0, volume->params.width - 1, volume->params.height - 1, volume->params.depth - 1);

	return octree.nodes;
}


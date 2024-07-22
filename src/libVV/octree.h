#ifndef __OCTREE_H__
#define __OCTREE_H__

struct volume;

struct octree_node
{
	struct
	{
		int	x;
		int	y;
		int	z;
	}begin, end;

	int	maximum;
	int	minimum;

	int	children[8];
};

struct octree_node* octree_create(const struct volume* volume);


#endif /* __OCTREE_H__ */

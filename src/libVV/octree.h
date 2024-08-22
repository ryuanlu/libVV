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

struct octree_node_queue
{
	int	count;
	int	max_count;
	const struct octree_node**	node;
};


struct octree_node* octree_create(const struct volume* volume);
struct octree_node_queue* octree_node_queue_create(void);
int octree_node_queue_destroy(struct octree_node_queue* queue);
int octree_node_queue_push(struct octree_node_queue* queue, const struct octree_node* octree_node);
void octree_find_value(const struct octree_node* root, const struct octree_node* octree_node, const int value, struct octree_node_queue* queue);


#endif /* __OCTREE_H__ */

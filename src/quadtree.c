#include "../include/quadtree.h"

#define NE 1
#define NW 2
#define SE 3
#define SW 4

static QNode* node_pool;
static size_t node_pool_len = DEFAULT_PARTICLES*NODES_MULT;
static size_t next_free_node = 0;

int init_node_pool()
{
	node_pool = malloc(node_pool_len*sizeof(QNode));
	if(node_pool == NULL)
	{
		return 1;
	}
	return 0;
}

QNode* get_next_node()
{
	if(next_free_node >= node_pool_len)
	{
		printf("too many nodes\n");
		return NULL;
	}
	QNode* node = &node_pool[next_free_node];
	next_free_node++;
	return node;
}


int build_quadtree(Entities* e)
{
	
}






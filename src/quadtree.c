#include "../include/quadtree.h"

#define NE 1
#define NW 2
#define SE 3
#define SW 4

static QNode* node_pool;
static size_t next_free_node = 0;

int init_node_pool()
{
	node_pool = malloc(DEFAULT_PARTICLES*NODES_MULT*sizeof(QNode));
	if(node_pool == NULL)
	{
		return 1;
	}
	return 0;
}

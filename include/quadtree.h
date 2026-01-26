#include "maths.h"
#include "constants.h"

#define NODES_MULT 6

typedef struct 
{
	int w_min, w_max, h_min, h_max;
}Quadrand;

typedef struct QNode
{
	struct QNode *ne, *nw, *se, *sw;
	float cum_mass;
	Vector2 com;
	//-1 if an internal node
	size_t e_idx;
	Quadrand quad;
} QNode;

QNode* get_clean_node(QNode* node_pool);


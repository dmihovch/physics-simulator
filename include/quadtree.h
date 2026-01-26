#ifndef QUADTREE_H
#define QUADTREE_H
#include "entities.h"
#include <stdio.h>
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

int init_node_pool();
QNode* get_next_node();
int build_quadtree(Entities* e);
void insert_qnode(QNode* root, QNode* node);

#endif //QUADTREE_H

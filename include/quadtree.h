#ifndef QUADTREE_H
#define QUADTREE_H
#include "entities.h"
#include <stdio.h>
#include <assert.h>
#include "maths.h"
#include "constants.h"
#define NODES_MULT 6
#define LEAF -1
#define NE 0
#define NW 1
#define SE 2
#define SW 3

typedef struct 
{
	int cx,cy,half;
}Quadrant;

typedef struct QNode
{
	//-1 if an internal node or leaf
	size_t entity;
	float cum_mass;
	Vector2 com;
	Quadrant quad;
	struct QNode* quads[4];
} QNode;

int init_node_pool();
QNode* get_next_node();
int build_quadtree(Entities* e);
void insert_qentity(QNode* root, Entities* e, size_t i);
int get_quadrant(QNode* node, Vector2 pos);
#endif //QUADTREE_H

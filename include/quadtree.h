#ifndef QUADTREE_H
#define QUADTREE_H
#include "entities.h"
#include <stdio.h>
#include <assert.h>
#include "maths.h"
#include "constants.h"
#define NODES_MULT 6

enum Direction 
{
	NE,
	NW,
	SE,
	SW
};

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
QNode* get_next_node(Quadrant parent, enum Direction direction);
int build_quadtree(Entities* e);
void insert_qentity(QNode* root, Entities* e, size_t i);
enum Direction get_quadrant(QNode* node, Vector2 pos);
#endif //QUADTREE_H

#ifndef QUADTREE_H
#define QUADTREE_H
#include "entities.h"
#include <stdio.h>
#include <assert.h>
#include "maths.h"
#include "constants.h"
#define NODES_MULT 64
#define TREE_DEPTH_LIMITER 0.001f

#define BH_THETA 0.5f
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
	int entity;
	float cum_mass;
	Vector2 com;
	Quadrant quad;
	struct QNode* quads[4];
} QNode;

void free_node_pool(void);
int init_node_pool(size_t nents);
QNode* get_next_node(Quadrant parent, enum Direction direction);
QNode* build_quadtree(Entities* e);
void insert_qentity(QNode* root, Entities* e, size_t i);
enum Direction get_quadrant(QNode* node, Vector2 pos);
int realloc_node_pool(size_t nents);
#endif //QUADTREE_H

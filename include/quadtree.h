#ifndef QUADTREE_H
#define QUADTREE_H
#include "entities.h"
#include <stdio.h>
#include <assert.h>
#include "maths.h"
#include "constants.h"
#define NODES_MULT 6
int init_node_pool(size_t nents);
QNode* get_next_node(Quadrant parent, enum Direction direction);
int build_quadtree(Entities* e);
void insert_qentity(QNode* root, Entities* e, size_t i);
enum Direction get_quadrant(QNode* node, Vector2 pos);
#endif //QUADTREE_H

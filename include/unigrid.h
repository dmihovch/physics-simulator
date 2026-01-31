#ifndef UNIGRID_H
#define UNIGRID_H
#include <stdlib.h>
#include "maths.h"
#include "entities.h"

typedef struct 
{
	size_t* entity;
	size_t count;
	size_t capacity;
} UCell;

typedef struct
{
	size_t* large_ents;
	size_t large_size;
	size_t large_cap;
	UCell* cells;
	size_t cells_rows;
	size_t cells_cols;
}UGrid;

int init_ugrid(Entities* e);
void free_ugrid(void);
int fill_ugrid(Entities* e);
void handle_entity_collisions_ugrid(Entities* e);
void handle_collisions_cells(Entities* e, size_t i, size_t cell_idx, bool same_cell);
void check_collision_sm_sm(Entities* e,size_t i);
size_t get_cell_idx(size_t row,size_t col,size_t cols);
void check_collision_sm_lg(Entities* e,size_t i);
//bodge!
#include <math.h>
void check_collision(Entities* e, size_t i, size_t j);
Vector2 check_collisions_circles(float* scalar_dist,Vector2 apos, float ar, Vector2 bpos, float br);
float calculate_impulse(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal);
bool collision_occured(Vector2 normal);
void handle_penetration(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal, float scalar_distance);
//!egdob
#endif //UNIGRID_H

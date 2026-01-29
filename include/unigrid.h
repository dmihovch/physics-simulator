#ifndef UNIGRID_H
#define UNIGRID_H
#include <stdlib.h>
#include "maths.h"
#include "entities.h"
#include "physics.h"

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
#endif //UNIGRID_H

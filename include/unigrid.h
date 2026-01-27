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
	UCell* small_grid;
	size_t grid_size;
}UGrid;

#endif //UNIGRID_H

#ifndef ENTITIES_H
#define ENTITIES_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "constants.h"
#include "maths.h"
#include "time.h"




typedef struct {
	size_t nents;
	Vector2* pos;
	Vector2* vel;
	Vector2* acc;
	float* r;
	float* m;
	Color* color;
	IVec2* gridpos;
	Vector2* oldpos;
}Entities;
int alloc_rand_entities(Entities* e);
void create_rand_entity(Entities* e, size_t i);
int realloc_rand_nentities(Entities* p, size_t new_n);
int check_free(Entities e);
float rand_float(float tmin, float tmax);
Color rand_color();

#endif //ENTITY_H

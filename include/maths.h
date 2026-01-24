#ifndef MATHS_H
#define MATHS_H
#include <stdlib.h>
#include "raylib.h"

//if I want to use this math library outside of raylib, need to delete the include and uncomment the struct below

/***************
typedef struct {
	float x,y;
}Vector2;
***************/

//returns a random float [tmin,tmax]
float rand_float(float tmin, float tmax);
float rand_float_nonzero(float tmin, float tmax);
float vec2_distance_squared(Vector2 a, Vector2 b);
void vec2_add_ip(Vector2* res, Vector2 v);
void vec2_sub_ip(Vector2* res, Vector2 v);
Vector2 vec2_sub(Vector2 a, Vector2 b);
void vec2_negate_ip(Vector2* v);
void vec2_zero(Vector2* v);
float vec2_dot(Vector2 a, Vector2 b);
void vec2_scalar_mult_ip(Vector2* v, float scalar);
Vector2 vec2_scalar_mult(Vector2 v, float scalar);
#endif //MATHS_H

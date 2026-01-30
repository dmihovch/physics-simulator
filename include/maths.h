#ifndef MATHS_H
#define MATHS_H
#include <stdlib.h>
#include <raylib.h>
//Since I use this with raylib a lot, I just wanted it to be seamlessly interoperable
#ifdef MATHS_VEC

typedef struct {
	float x,y;
}Vector2;

#endif //MATHS_VEC
typedef struct {
	int x,y;
}IVec2;

typedef struct 
{
	int lb,ub;
}Range;

//returns a random float [tmin,tmax]
float rand_float(float tmin, float tmax);
float rand_float_nonzero(float tmin, float tmax);
float vec2_distance_squared(Vector2 a, Vector2 b);
void vec2_add_ip(Vector2* res, Vector2 v);
void vec2_sub_ip(Vector2* res, Vector2 v);
int scale_value(int v, Range rold, Range rnew);
Vector2 vec2_sub(Vector2 a, Vector2 b);
void vec2_negate_ip(Vector2* v);
void vec2_zero(Vector2* v);
float vec2_dot(Vector2 a, Vector2 b);
void vec2_scalar_mult_ip(Vector2* v, float scalar);
size_t matrix_to_linear_idx(IVec2 coords, size_t cols);
Vector2 vec2_scalar_mult(Vector2 v, float scalar);
#endif //MATHS_H

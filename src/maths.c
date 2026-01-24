#include "../include/maths.h"
#include <raylib.h>

float vec2_distance_squared(Vector2 a, Vector2 b)
{
	Vector2 delta = vec2_sub(b,a);
	return vec2_dot(delta,delta);
}

float rand_float_nonzero(float tmin, float tmax)
{
	float rand = rand_float(tmin,tmax);
	if(rand == 0.0f)
	{
		//lowkenually arbitrary value
		return 0.0001;
	}
	return rand;
}
float rand_float(float tmin, float tmax)
{
	float rmin = 0.;
	float rmax = 1.;
	float ret = (float)rand()/RAND_MAX;
	ret = ret - rmin;
	ret = ret / (rmax - rmin);
	return ret * (tmax - tmin) + tmin;
}

void vec2_negate_ip(Vector2* v)
{
	v->x = -v->x;
	v->y = -v->y;
}

void vec2_add_ip(Vector2* res, Vector2 v)
{
	res->x += v.x;
	res->y += v.y;
}

void vec2_sub_ip(Vector2 *res, Vector2 v)
{
	res->x -= v.x;
	res->y -= v.y;
}

void vec2_zero(Vector2* v)
{
	v->x = 0;
	v->y = 0;
}

float vec2_dot(Vector2 a, Vector2 b)
{
	return (a.x * b.x) + (a.y * b.y);
}

Vector2 vec2_sub(Vector2 a, Vector2 b)
{
	return (Vector2)
	{
		a.x - b.x,
		a.y - b.y,
	};
}

void vec2_scalar_mult_ip(Vector2* v, float scalar)
{
	v->x *= scalar;
	v->y *= scalar;
}

Vector2 vec2_scalar_mult(Vector2 v, float scalar)
{
	return (Vector2)
	{
		v.x*scalar,
		v.y*scalar
	};
}

#ifndef PARTICLE_H
#define PARTICLE_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "constants.h"
#include "maths.h"
#include "time.h"
typedef struct {
	Vector2 pos;
	Vector2 vel;
	Vector2 acc;
	float r;
	float m;
	Color color;
}Particle;
Particle* alloc_rand_nparticles(int n);
Particle create_rand_particle();
Particle* realloc_rand_nparticles(Particle* p, int new_n, int old_n);
float rand_float(float tmin, float tmax);
Color rand_color();
#endif //PARTICLE_H

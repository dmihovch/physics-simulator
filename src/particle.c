#include "../include/particle.h"
#include <raylib.h>
#include <raymath.h>

Particle* alloc_rand_nparticles(int n)
{
	Particle* p = malloc(n*sizeof(Particle));
	if(!p) 
	{
		return NULL;
	}

	for(int i = 0; i < n; ++i)
	{
		p[i] = create_rand_particle();
	}

	return p;

}

Particle create_rand_particle()
{
	return (Particle)
	{
		.pos = (Vector2){rand_float(0,WIDTH),rand_float(0,HEIGHT)},
		.vel = (Vector2){rand_float(-20.0,20.0),rand_float(-20.0,20.0)},
		.acc = (Vector2){0,0},
		.r = 5.,
		.m = .001,
		.color = rand_color(),
	};
}

Particle* realloc_rand_nparticles(Particle* p, int new_n, int old_n)
{
	Particle* new_p  = (Particle*)realloc(p,new_n*sizeof(Particle));
	if(new_p == NULL)
	{
		return NULL;
	}
	for(int i = old_n; i<new_n; ++i)
	{
		new_p[i] = create_rand_particle();
	}
	return new_p;
}


Color rand_color()
{
	Color colors[] = {RAYWHITE,RED,BLUE,PURPLE,PINK,YELLOW,ORANGE,GREEN};
	return colors[rand()%8];
}


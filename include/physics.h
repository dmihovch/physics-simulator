#ifndef PHYSICS_H
#define PHYSICS_H
#include "particle.h"
#include "constants.h"
#include "maths.h"
#include <math.h>
typedef struct
{
	float nparticles; 
	float gravity;
	float timestep;
	// float rand_vel_range; //+-rand_vel_range from 0
	// float rand_r_range; // 2 - rand_r_range
	// float rand_m_range; // 0.001 - rand_m_range
}Options;



void update_particles(Particle* p, Options opts);
void reset_accelerations(Particle* p, Options opts);
void accumulate_forces(Particle* p, Options opts);
void move_particles_handle_walls(Particle* p, Options opts);
void handle_particle_collisions(Particle* p, Options opts);
Vector2 check_collisions_circles(float* scalar_dist,Vector2 apos, float ar, Vector2 bpos, float br);
float calculate_impulse(Particle a, Particle b, Vector2 normal);
bool collision_occured(Vector2 normal);
void handle_penetration(Particle* a, Particle* b,Vector2 normal, float scalar_distance);
#endif //PHYSICS_H

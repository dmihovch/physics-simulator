#ifndef PHYSICS_H
#define PHYSICS_H
#include "entities.h"
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



void update_entities(Entities* e, Options opts);
void reset_accelerations(Entities* e);
void accumulate_forces(Entities* e, Options opts);
void move_entities_handle_walls(Entities* e, Options opts);
void handle_entity_collisions(Entities* e);
Vector2 check_collisions_circles(float* scalar_dist,Vector2 apos, float ar, Vector2 bpos, float br);
float calculate_impulse(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal);
bool collision_occured(Vector2 normal);
void handle_penetration(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal, float scalar_distance);
#endif //PHYSICS_H

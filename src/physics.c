#include "../include/physics.h"



void update_entities(Entities* e, Options opts)
{
	reset_accelerations(e);
	accumulate_forces(e,opts);
	move_entities_handle_walls(e,opts);
	handle_entity_collisions(e);
}

void reset_accelerations(Entities* e)
{
	for(size_t i = 0; i<e->n_ents; ++i)
	{
		vec2_zero(&e->acc[i]);
	}
}

void accumulate_forces(Entities* e, Options opts)
{
	for(size_t i = 0; i<e->n_ents; ++i)
	{
		for(size_t j = i + 1; j<e->n_ents; ++j)
		{
			//calculate the forces for both i and j
			//inverse square law
			Vector2 delta = vec2_sub(e->pos[j], e->pos[i]);
			float distsq = vec2_dot(delta, delta);
			float dist = sqrtf(distsq);
			Vector2 rhat = vec2_scalar_mult(delta, 1/dist);
			float grav_mass_dist = (opts.gravity*e->m[i]*e->m[j]) / distsq;
			Vector2 force = vec2_scalar_mult(rhat, grav_mass_dist);
			Vector2 force_mass_pofi = vec2_scalar_mult(force, 1/e->m[i]);
			Vector2 force_mass_pofj = vec2_scalar_mult(force, 1/e->m[j]);
			vec2_add_ip(&e->acc[i], force_mass_pofi);
			vec2_sub_ip(&e->acc[j], force_mass_pofj);
		}
	}
}

void move_entities_handle_walls(Entities* e, Options opts)
{
	for(size_t i = 0; i<e->n_ents; ++i)
	{
		vec2_add_ip(&e->vel[i],vec2_scalar_mult(e->acc[i], opts.timestep));
		vec2_add_ip(&e->pos[i],vec2_scalar_mult(e->vel[i], opts.timestep));

		if(e->pos[i].x + e->r[i] > WIDTH)
		{
			e->pos[i].x = WIDTH - e->r[i];
			e->vel[i].x = -e->vel[i].x * ELASTICITY;
		}
		else if(e->pos[i].x - e->r[i] < 0)
		{
			e->pos[i].x = e->r[i];
			e->vel[i].x = -e->vel[i].x * ELASTICITY;
		}
		if(e->pos[i].y + e->r[i] > HEIGHT)
		{
			e->pos[i].y = HEIGHT - e->r[i];
			e->vel[i].y = -e->vel[i].y * ELASTICITY;
		}
		else if(e->pos[i].y - e->r[i] < 0)
		{
			e->pos[i].y = e->r[i];
			e->vel[i].y = -e->vel[i].y * ELASTICITY;
		}
	}

}

void handle_entity_collisions(Entities* e)
{
	for(size_t i = 0; i<e->n_ents; ++i)
	{
		for(size_t j = i+1; j<e->n_ents; ++j)
		{
			float scalar_dist;	
			Vector2 normal = check_collisions_circles(&scalar_dist,e->pos[i], e->r[i], e->pos[j], e->r[j]);
			if(collision_occured(normal)){
				float impulse = calculate_impulse(e, i, j, normal);
				Vector2 impulse_vector = vec2_scalar_mult(normal, impulse);
				//this is messy, I don't like it, but it's not the point
				vec2_add_ip(&e->vel[i], vec2_scalar_mult(impulse_vector, 1/e->m[i]));
				vec2_add_ip(&e->vel[j], vec2_scalar_mult(impulse_vector, -(1/e->m[j])));
				handle_penetration(e, i,j, normal, scalar_dist);
			}
		}
	}
}

void handle_penetration(Entities* e,size_t a_idx, size_t b_idx,	Vector2 normal, float scalar_distance)
{
	float penetration_distance = e->r[a_idx] + e->r[b_idx];
	if(scalar_distance > 0.)
	{
		penetration_distance -= scalar_distance;
	}
	Vector2 scaled_normal = vec2_scalar_mult(normal, penetration_distance*PERCENT_CORRECTION);
	float mass_for_a = e->m[b_idx]/(e->m[a_idx] + e->m[b_idx]);
	float mass_for_b = e->m[a_idx]/(e->m[a_idx] + e->m[b_idx]);
	Vector2 a_correction = vec2_scalar_mult(scaled_normal, mass_for_a);
	Vector2 b_correction = vec2_scalar_mult(scaled_normal, mass_for_b);
	vec2_sub_ip(&e->pos[a_idx], a_correction);
	vec2_add_ip(&e->pos[b_idx], b_correction);
}
float calculate_impulse(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal)
{
	Vector2 relative_vel = vec2_sub(e->vel[a_idx], e->vel[b_idx]);
	float relative_normal = vec2_dot(relative_vel,normal);
	float inverse_masses = (1/e->m[a_idx]) + (1/e->m[b_idx]);
	float neg1plusE = -(1 + ELASTICITY);
	return (neg1plusE * relative_normal) / inverse_masses;
}

bool collision_occured(Vector2 normal)
{
	return (normal.x != 0 && normal.y != 0);
}

Vector2 check_collisions_circles(float* scalar_dist,Vector2 apos, float ar, Vector2 bpos, float br)
{

	*scalar_dist = -1;
	Vector2 delta = vec2_sub(bpos, apos);
	float distsq = vec2_dot(delta,delta);
	float rsq = (ar+br) * (ar+br);
	if(distsq > rsq)
	{
		return (Vector2){0,0};
	}
	if(distsq == 0.)
	{
		//not sure if I have to calculate the peenetration of this case
		*scalar_dist = 0.;
		return (Vector2)
		{
			1,0
		};
	} 

	float dist = sqrtf(distsq);
	*scalar_dist = dist;
	return (Vector2){delta.x/dist, delta.y/dist};
}















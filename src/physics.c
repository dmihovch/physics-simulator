#include "../include/physics.h"
#include <raylib.h>



void update_particles(Particle* p, Options opts)
{
	reset_accelerations(p,opts);
	accumulate_forces(p,opts);
	move_particles_handle_walls(p,opts);
	handle_particle_collisions(p,opts);
}

void reset_accelerations(Particle* p, Options opts)
{
	for(int i = 0; i<opts.nparticles; ++i)
	{
		vec2_zero(&p[i].acc);
	}
}

void accumulate_forces(Particle* p, Options opts)
{
	for(int i = 0; i<opts.nparticles; ++i)
	{
		for(int j = i + 1; j<opts.nparticles; ++j)
		{
			//calculate the forces for both i and j
			//inverse square law
			Vector2 delta = vec2_sub(p[j].pos, p[i].pos);
			float distsq = vec2_dot(delta, delta);
			float dist = sqrtf(distsq);
			Vector2 rhat = vec2_scalar_mult(delta, 1/dist);
			float grav_mass_dist = (opts.gravity*p[i].m*p[j].m) / distsq;
			Vector2 force = vec2_scalar_mult(rhat, grav_mass_dist);
			Vector2 force_mass_pofi = vec2_scalar_mult(force, 1/p[i].m);
			Vector2 force_mass_pofj = vec2_scalar_mult(force, 1/p[j].m);
			vec2_add_ip(&p[i].acc, force_mass_pofi);
			vec2_sub_ip(&p[j].acc, force_mass_pofj);
		}
	}
}

void move_particles_handle_walls(Particle* p, Options opts)
{
	for(int i = 0; i<opts.nparticles; ++i)
	{
		vec2_add_ip(&p[i].vel,vec2_scalar_mult(p[i].acc, opts.timestep));
		vec2_add_ip(&p[i].pos,vec2_scalar_mult(p[i].vel, opts.timestep));

		if(p[i].pos.x + p[i].r > WIDTH)
		{
			p[i].pos.x = WIDTH - p[i].r;
			p[i].vel.x = -p[i].vel.x * ELASTICITY;
		}
		else if(p[i].pos.x - p[i].r < 0)
		{
			p[i].pos.x = p[i].r;
			p[i].vel.x = -p[i].vel.x * ELASTICITY;
		}
		if(p[i].pos.y + p[i].r > HEIGHT)
		{
			p[i].pos.y = HEIGHT - p[i].r;
			p[i].vel.y = -p[i].vel.y * ELASTICITY;
		}
		else if(p[i].pos.y - p[i].r < 0)
		{
			p[i].pos.y = p[i].r;
			p[i].vel.y = -p[i].vel.y * ELASTICITY;
		}
	}

}

void handle_particle_collisions(Particle* p, Options opts)
{
	for(int i = 0; i < opts.nparticles; ++i)
	{
		for(int j = i+1; j<opts.nparticles; ++j)
		{
			float scalar_dist;	
			Vector2 normal = check_collisions_circles(&scalar_dist,p[i].pos, p[i].r, p[j].pos, p[j].r);
			if(collision_occured(normal)){
				float impulse = calculate_impulse(p[i], p[j], normal);
				Vector2 impulse_vector = vec2_scalar_mult(normal, impulse);
				//this is messy, I don't like it, but it's not the point
				vec2_add_ip(&p[i].vel, vec2_scalar_mult(impulse_vector, 1/p[i].m));
				vec2_add_ip(&p[j].vel, vec2_scalar_mult(impulse_vector, -(1/p[j].m)));
				handle_penetration(&p[i], &p[j], normal, scalar_dist);
			}
		}
	}
}

void handle_penetration(Particle* a, Particle* b,Vector2 normal, float scalar_distance)
{
	float penetration_distance = a->r + b->r;
	if(scalar_distance > 0.)
	{
		penetration_distance -= scalar_distance;
	}
	Vector2 scaled_normal = vec2_scalar_mult(normal, penetration_distance*PERCENT_CORRECTION);
	float mass_for_a = b->m/(a->m + b->m);
	float mass_for_b = a->m/(a->m + b->m);
	Vector2 a_correction = vec2_scalar_mult(scaled_normal, mass_for_a);
	Vector2 b_correction = vec2_scalar_mult(scaled_normal, mass_for_b);
	vec2_sub_ip(&a->pos, a_correction);
	vec2_add_ip(&b->pos, b_correction);
}
float calculate_impulse(Particle a, Particle b, Vector2 normal)
{
	Vector2 relative_vel = vec2_sub(a.vel, b.vel);
	float relative_normal = vec2_dot(relative_vel,normal);
	float inverse_masses = (1/a.m) + (1/b.m);
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















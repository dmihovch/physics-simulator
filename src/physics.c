#include "../include/physics.h"



void update_entities(Entities* e, Options opts)
{
	// printf("in update\n");
	reset_accelerations(e);
	// printf("past reset\n");
	accumulate_forces(e,opts);
	// printf("past accumulate\n");
	move_entities(e,opts);
	// printf("past move\n");
	handle_entity_collisions_ugrid(e);
	// printf("past handle\n");
	handle_walls(e);
}

void reset_accelerations(Entities* e)
{
	memset(e->acc,0,e->nents*sizeof(Vector2));
}


void grav_force_calculation(Entities* e, QNode* node, size_t i, float gravity, Vector2 delta, float d2)
{
	float dist = sqrtf(d2);
	Vector2 rhat = vec2_scalar_mult(delta, 1/dist);
	float grav_mass_dist = (gravity * e->m[i] * node->cum_mass) / (d2 + SOFTENING);
	Vector2 force = vec2_scalar_mult(rhat, grav_mass_dist);
	Vector2 force_mass = vec2_scalar_mult(force, 1/e->m[i]);
	vec2_add_ip(&e->acc[i], force_mass);
	return;
}

void accumulate_forces_from_tree(Entities* e, QNode* node, int i, float gravity)
{
	if(node == NULL || node->entity == i) //refactor
	{
		return;
	}
	
	Vector2 delta = vec2_sub(vec2_scalar_mult(node->com, 1.0f/node->cum_mass),e->pos[i]);
	float d2 = vec2_dot(delta,delta);
	if(node->entity != -1)
	{
		grav_force_calculation(e,node,i,gravity,delta,d2);
		return;
	}
	float width_s2 = (node->quad.half*2)*(node->quad.half*2);
	if(d2 == 0. || width_s2 >= (BH_THETA*BH_THETA) * d2)
	{
		accumulate_forces_from_tree(e,node->quads[NE],i,gravity);
		accumulate_forces_from_tree(e,node->quads[NW],i,gravity);
		accumulate_forces_from_tree(e,node->quads[SE],i,gravity);
		accumulate_forces_from_tree(e,node->quads[SW],i,gravity);
		return;
	}
	grav_force_calculation(e,node,i,gravity,delta,d2);
	return;
}

void accumulate_forces(Entities* e, Options opts)
{
	// printf("in accumulate\n");

	QNode* root = build_quadtree(e);
	if(root == NULL)
	{
		printf("failed to build quadtree\n");
		return;
	}
	// printf("past build\n");
	for(size_t i = 0; i<e->nents; ++i)
	{
		// printf("in %ld... ",i);
		accumulate_forces_from_tree(e,root,(int)i,opts.gravity);
		// printf("past %ld\n",i);
	}
	
}

void handle_walls(Entities* e)
{
	for(size_t i = 0; i<e->nents; ++i)
	{
		if(e->pos[i].x + e->r[i] > SIM_MAX_WIDTH_COORD)
		{
			e->pos[i].x = SIM_MAX_WIDTH_COORD - e->r[i];
			e->vel[i].x = -e->vel[i].x * ELASTICITY;
		}
		else if(e->pos[i].x - e->r[i] < SIM_MIN_WIDTH_COORD)
		{
			e->pos[i].x = SIM_MIN_WIDTH_COORD + e->r[i];
			e->vel[i].x = -e->vel[i].x * ELASTICITY;
		}
		if(e->pos[i].y + e->r[i] > SIM_MAX_HEIGHT_COORD)
		{
			e->pos[i].y = SIM_MAX_HEIGHT_COORD - e->r[i];
			e->vel[i].y = -e->vel[i].y * ELASTICITY;
		}
		else if(e->pos[i].y - e->r[i] < SIM_MIN_HEIGHT_COORD)
		{
			e->pos[i].y = SIM_MIN_HEIGHT_COORD + e->r[i];
			e->vel[i].y = -e->vel[i].y * ELASTICITY;
		}
	}
}

void move_entities(Entities* e, Options opts)
{
	for(size_t i = 0; i<e->nents; ++i)
	{
		vec2_add_ip(&e->vel[i],vec2_scalar_mult(e->acc[i], opts.timestep));
		vec2_add_ip(&e->pos[i],vec2_scalar_mult(e->vel[i], opts.timestep));

	}

}
















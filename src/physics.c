#include "../include/physics.h"



void update_entities(Entities* e, Options opts)
{
	// printf("in update\n");
	reset_accelerations(e);
	// printf("past reset\n");
	accumulate_forces(e,opts);
	// printf("past accumulate\n");
	move_entities_handle_walls(e,opts);
	// printf("past move\n");
	handle_entity_collisions(e);
	// printf("past handle\n");
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
	if(node == NULL || node->entity == i)
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

	build_quadtree(e);
	// printf("past build\n");
	for(size_t i = 0; i<e->nents; ++i)
	{
		// printf("in %ld... ",i);
		accumulate_forces_from_tree(e,e->root,(int)i,opts.gravity);
		// printf("past %ld\n",i);
	}
	
}

void move_entities_handle_walls(Entities* e, Options opts)
{
	for(size_t i = 0; i<e->nents; ++i)
	{
		vec2_add_ip(&e->vel[i],vec2_scalar_mult(e->acc[i], opts.timestep));
		vec2_add_ip(&e->pos[i],vec2_scalar_mult(e->vel[i], opts.timestep));

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

static pthread_mutex_t hec_pos = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t hec_vel = PTHREAD_MUTEX_INITIALIZER;

void* threaded_entity_collisions(void* payload)
{
	// printf("thread in entity col\n");
	ThreadPayload* tpay = (ThreadPayload*)payload;
	Entities* e = tpay->e;
	size_t start = tpay->start;
	size_t end = tpay->end;
	
	Vector2 tmp_pos[e->nents];
	Vector2 tmp_vel[e->nents];

	for(size_t i = start; i<end; i++)
	{
		tmp_vel[i] = (Vector2){0,0};
		tmp_pos[i] = (Vector2){0,0};

		for(size_t j = i+1; j<e->nents; ++j)
		{
			float scalar_dist;	
			pthread_mutex_lock(&hec_pos);
			Vector2 normal = check_collisions_circles(&scalar_dist,e->pos[i], e->r[i], e->pos[j], e->r[j]);
			pthread_mutex_unlock(&hec_pos);
			if(collision_occured(normal)){
				float impulse = calculate_impulse(e, i, j, normal);
				Vector2 impulse_vector = vec2_scalar_mult(normal, impulse);
				//this is messy, I don't like it, but it's not the point
				pthread_mutex_lock(&hec_vel);
				vec2_add_ip(&e->vel[i], vec2_scalar_mult(impulse_vector, 1/e->m[i]));
				vec2_add_ip(&e->vel[j], vec2_scalar_mult(impulse_vector, -(1/e->m[j])));
				pthread_mutex_unlock(&hec_vel);
				handle_penetration(e, i,j, normal, scalar_dist);
			}
		}
	}


	pthread_mutex_lock(&hec_vel);
	pthread_mutex_lock(&hec_pos);
	for(size_t i = start; i<end; i++)
	{
		for(size_t j = i+1; j<e->nents; ++i)
		{
			vec2_add_ip(&e->vel[i], tmp_vel[i]);
			vec2_add_ip(&e->vel[j],tmp_vel[j]);
			vec2_sub_ip(&e->pos[i], tmp_pos[i]);
			vec2_add_ip(&e->pos[j], tmp_pos[j]);
		}
	}
	pthread_mutex_unlock(&hec_pos);
	pthread_mutex_unlock(&hec_vel);

	return NULL;
}

void handle_entity_collisions(Entities* e)
{
	for(size_t i = 0; i<e->nents; ++i)
	{
		for(size_t j = i+1; j<e->nents; ++j)
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
/*
 *
 *
 *
 *
 * TS broken as hell and ugly too omm
void handle_entity_collisions(Entities* e)
{
	pthread_t threads[THREADS];
	ThreadPayload payload[THREADS];

	int total_weight = THREADS * (THREADS + 1) / 2;

	int start = 0;
	for(int i = 0; i < THREADS; ++i)
	{
		int range_weight = i+1;
		int range_length = (range_weight * e->nents) / total_weight;
		if(i == THREADS - 1)
		{
			range_length = e->nents - start;
		}
		int end = start + range_length;
		payload[i].start = start;
		payload[i].end = end;
		payload[i].e = e;

		printf("payload[%d].start = %d\npayload[%d].end = %d\n",i,start,i,end);
		
		int err = pthread_create(&threads[i],NULL,threaded_entity_collisions,&payload[i]);
		if(err)
		{
			printf("thread %d failed to launch, handle_entity_collisions\n",i);
		}

		start = end;
	}
	for(int i = 0; i<THREADS; ++i)
	{
		pthread_join(threads[i],NULL);
	}

}
*/
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
	pthread_mutex_lock(&hec_pos);
	vec2_sub_ip(&e->pos[a_idx], a_correction);
	vec2_add_ip(&e->pos[b_idx], b_correction);
	pthread_mutex_unlock(&hec_pos);
}
float calculate_impulse(Entities* e, size_t a_idx, size_t b_idx, Vector2 normal)
{
	pthread_mutex_lock(&hec_vel);
	Vector2 relative_vel = vec2_sub(e->vel[a_idx], e->vel[b_idx]);
	pthread_mutex_unlock(&hec_vel);
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















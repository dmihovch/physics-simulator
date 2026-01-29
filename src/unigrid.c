#include "../include/unigrid.h"



static UGrid grid;   


int init_ugrid(Entities* e)
{
	size_t nents = e->nents;
	grid.large_ents = malloc(nents*sizeof(size_t));
	if(grid.large_ents == NULL)
	{
		return 1;
	}
	grid.large_size = 0;
	grid.large_cap = nents;

	size_t cells_rows = SIM_DIM/SMALL_PARTICLE_CUTOFF;
	size_t cells_cols = SIM_DIM/SMALL_PARTICLE_CUTOFF;
	size_t num_cells = cells_rows * cells_cols;
	grid.cells = malloc(num_cells*sizeof(UCell));
	if(grid.cells == NULL)
	{
		free(grid.large_ents);
		return 2;
	}
	grid.cells_rows = cells_rows;
	grid.cells_cols = cells_cols;
	
	for(size_t i = 0; i<num_cells; ++i)
	{
		grid.cells[i].entity = malloc(nents*sizeof(size_t));
		if(grid.cells[i].entity == NULL)
		{
			for(size_t j = 0; j<i; j++)
			{
				free(grid.cells[i].entity);
			}
			break;
		}
	}
	if(grid.cells[0].entity == NULL)
	{
		free(grid.cells);
		free(grid.large_ents);
		return 3;
	}

	return 0;

}

void free_ugrid()
{
	if(grid.large_ents != NULL)
	{
		free(grid.large_ents);
	}
	if(grid.cells != NULL)
	{
		size_t gridsz = grid.cells_rows*grid.cells_cols;
		for(size_t i = 0; i<gridsz; ++i)
		{
			if(grid.cells[i].entity != NULL)
			{
				free(grid.cells[i].entity);
			}
		}
		free(grid.cells);
	}
}


int fill_ugrid(Entities* e)
{
	grid.large_size = 0;
	Range old_y = (Range)
	{
		.lb = SIM_MIN_HEIGHT_COORD,
		.ub = SIM_MAX_HEIGHT_COORD
	};
	Range old_x = (Range)
	{
		.lb = SIM_MIN_WIDTH_COORD,
		.ub = SIM_MAX_WIDTH_COORD
	};
	Range new = (Range)
	{
		.lb = 0,
		.ub = SIM_DIM
	};

	IVec2 iscaled_pos;
	for(size_t i = 0; i<e->nents; ++i)
	{
		if(e->r[i] > SMALL_PARTICLE_CUTOFF)
		{
			e->grididx[i] = -1;
			if(grid.large_size >= grid.large_cap)
			{
				size_t* tmp = realloc(grid.large_ents, grid.large_cap*2*sizeof(size_t));
				if(tmp == NULL)
				{
					printf("failed to realloc grid large ents\n");
					return 1;
				}
				grid.large_cap*=2;
				grid.large_ents = tmp;
			}
			grid.large_ents[grid.large_size] = i;
			grid.large_size++;
			continue;
		}

		iscaled_pos = (IVec2)
		{
			.x = scale_value((int)e->pos[i].x,old_x,new),
			.y = scale_value((int)e->pos[i].y,old_y,new)
		};

		iscaled_pos.x /= SMALL_PARTICLE_CUTOFF;
		iscaled_pos.y /= SMALL_PARTICLE_CUTOFF;



		size_t idx = iscaled_pos.x * grid.cells_cols + iscaled_pos.y;
		e->grididx[i] = idx;
		if(grid.cells[idx].count >= grid.cells[idx].capacity)
		{
			size_t* tmp = realloc(grid.cells[idx].entity, grid.cells[idx].capacity * 2 * sizeof(size_t));
			if(tmp == NULL)
			{
				printf("failed to realloc grid.cells[%d]\n",(int)idx);
				return 2;
			}
			grid.cells[idx].entity = tmp;
			grid.cells[idx].capacity*=2;
		}
		grid.cells[idx].entity[grid.cells[idx].count] = i;
		grid.cells[idx].count++;
	}
	return 0;
}

void check_collision_cells(Entities* e,size_t i, UCell cell)
{

}
//dont think i need grididx
void check_collision_sm_sm(Entities* e,size_t i)
{
	
	size_t idx = e->grididx[i];
	size_t cols = grid.cells_cols;
	size_t grid_size = grid.cells_cols*grid.cells_rows;

	size_t topleft = (idx-cols)-1;
	if(topleft >= 0)
	{
	check_collision_cells(e,i,grid.cells[topleft]);
	}
	size_t topmid = idx-cols;
	if(topmid >= 0)
	{
	check_collision_cells(e,i,grid.cells[topmid]);
	}
	size_t topright = (idx-cols)+1;
	if(topright >= 0)
	{
		check_collision_cells(e,i,grid.cells[topright]);
	}
	size_t midleft = idx-1;
	if(midleft >= 0)
	{
		check_collision_cells(e,i,grid.cells[midleft]);
	}
	check_collision_cells(e,i,grid.cells[idx]);
	size_t midright = idx+1;
	if(midright < grid_size)
	{
	check_collision_cells(e,i,grid.cells[midright]);
	}
	size_t botleft = (idx+cols)-1;
	if(botleft < grid_size)
	{
	check_collision_cells(e,i,grid.cells[botleft]);
	}
	size_t botmid = idx+cols;
	if(botmid < grid_size)
	{
	check_collision_cells(e,i,grid.cells[botmid]);
	}
	size_t botright = (idx+cols)+1;
	if(botright < grid_size)
	{
	check_collision_cells(e,i,grid.cells[botright]);
	}
}

void check_collision_sm_lg(Entities* e,size_t i)
{

}

void handle_collisions_ugrid(Entities* e)
{

	fill_ugrid(e);

	for(size_t i = 0; i<e->nents; ++i)
	{
		
		if(e->r[i] > SMALL_PARTICLE_CUTOFF)
		{
			continue;
		}
		check_collision_sm_sm(e,i);
		check_collision_sm_lg(e,i);

	}

	for(size_t i = 0; i<grid.large_size; ++i)
	{
		for(size_t j = i+1; j<grid.large_size; ++j)
		{
			check_collision(e,i,j);
		}
	}
		
}


//this is a total bodge, but...


void check_collision(Entities* e, size_t i, size_t j)
{
	float scalar_dist;	
	Vector2 normal = check_collisions_circles(&scalar_dist,e->pos[i], e->r[i], e->pos[j], e->r[j]);
	if(collision_occured(normal))
	{
		float impulse = calculate_impulse(e, i, j, normal);
		Vector2 impulse_vector = vec2_scalar_mult(normal, impulse);
		//this is messy, I don't like it, but it's not the point
		vec2_add_ip(&e->vel[i], vec2_scalar_mult(impulse_vector, 1/e->m[i]));
		handle_penetration(e, i,j, normal, scalar_dist);
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


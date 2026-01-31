#include "../include/unigrid.h"



static UGrid grid;   


#define DEFAULT_CELL_SIZE_CAP 8

int init_ugrid(Entities* e)
{
	size_t nents = e->nents;
	grid.large_ents = malloc(nents*sizeof(size_t));
	if(grid.large_ents == NULL)
	{
		grid.large_ents = NULL;
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
		grid.large_ents = NULL;
		return 1;
	}
	grid.cells_rows = cells_rows;
	grid.cells_cols = cells_cols;
	
	int failed_at_idx = -1;
	for(int i = 0; i<(int)num_cells; ++i)
	{
		grid.cells[i].capacity = DEFAULT_CELL_SIZE_CAP;
		grid.cells[i].count = 0;
		grid.cells[i].entity = malloc(grid.cells[i].capacity*sizeof(size_t));
		if(grid.cells[i].entity == NULL)
		{
			failed_at_idx = i;
			break;
		}
	}
	if(failed_at_idx != -1)
	{
		for(int i = 0; i<failed_at_idx; ++i)
		{
			free(grid.cells[i].entity);
		}
		free(grid.cells);
		free(grid.large_ents);
		return 1;
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
	for(size_t i = 0; i<grid.cells_cols*grid.cells_rows; ++i)
	{
		grid.cells[i].count = 0;
	}

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
			e->gridpos[i] = (IVec2){-1,-1};
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
		if(iscaled_pos.x >= (int)grid.cells_cols)
		{
			iscaled_pos.x = (int)grid.cells_cols-1;
		}
		if(iscaled_pos.x < 0)
		{
			iscaled_pos.x = 0;
		}

		iscaled_pos.y /= SMALL_PARTICLE_CUTOFF;
		if(iscaled_pos.y >= (int)grid.cells_rows)
		{
			iscaled_pos.y = (int)grid.cells_rows-1;
		}
		if(iscaled_pos.y < 0)
		{
			iscaled_pos.y = 0;
		}

		size_t idx = matrix_to_linear_idx(iscaled_pos,grid.cells_cols);
		e->gridpos[i] = iscaled_pos;
		if(grid.cells[idx].count >= grid.cells[idx].capacity)
		{
			if(grid.cells[idx].entity == NULL)
			{
				printf("NULL?????\n");
			}
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

void check_collision_cells(Entities* e,size_t i, UCell cell, bool same_cell)
{
	for(size_t j_i = 0; j_i<cell.count; ++j_i)
	{
		size_t j = cell.entity[j_i];
		if(same_cell && i >= j)
		{
			continue;
		}

		check_collision(e,i,cell.entity[j_i]);
	}
}


//dont think i need grididx
void check_collision_sm_sm(Entities* e,size_t i)
{
	
	IVec2 pos = e->gridpos[i];
	int col = pos.x;
	int row = pos.y;

	int ix;
	int iy;
	for(int drow = 0; drow<=1; ++drow)
	{
		for(int dcol = -1; dcol<=1; ++dcol)
		{
			if(drow == 0 && dcol == -1)
			{
				continue;
			}
			iy = row + drow;
			ix = col + dcol;
			if((iy >= 0 && iy < (int)grid.cells_rows) && (ix >= 0 && ix <(int)grid.cells_cols))
			{
				bool same_cell = (drow == 0 && dcol == 0);
				check_collision_cells(e,i,grid.cells[matrix_to_linear_idx((IVec2){ix,iy},grid.cells_cols)],same_cell);
			}
		}
	}
}

void check_collision_sm_lg(Entities* e,size_t i)
{
	for(size_t j_i = 0; j_i<grid.large_size; j_i++)
	{

		size_t large_idx = grid.large_ents[j_i];
		if(i < large_idx)
		{
			check_collision(e,i,large_idx);
		}
		else
		{
			check_collision(e,large_idx,i);
		}

	}
}

void handle_entity_collisions_ugrid(Entities* e)
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

	if(i == j)
	{
		return;
	}

	float scalar_dist;	
	Vector2 normal = check_collisions_circles(&scalar_dist,e->pos[i], e->r[i], e->pos[j], e->r[j]);
	if(collision_occured(normal))
	{
		float impulse = calculate_impulse(e, i, j, normal);
		Vector2 impulse_vector = vec2_scalar_mult(normal, impulse);
		//this is messy, I don't like it, but it's not the point
		vec2_add_ip(&e->vel[i], vec2_scalar_mult(impulse_vector, 1/e->m[i]));
		vec2_sub_ip(&e->vel[j], vec2_scalar_mult(impulse_vector, 1/e->m[j]));
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


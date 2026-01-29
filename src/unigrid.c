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

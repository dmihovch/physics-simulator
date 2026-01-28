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

	size_t num_cells = (SIM_DIM/SMALL_PARTICLE_CUTOFF) * (SIM_DIM/SMALL_PARTICLE_CUTOFF);
	grid.cells = malloc(num_cells*sizeof(UCell));
	if(grid.cells == NULL)
	{
		free(grid.large_ents);
		return 2;
	}
	
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
		for(size_t i = 0; i<grid.cell_size; ++i)
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
	for(size_t i = 0; i<e->nents; ++i)
	{
		//gonna figure ts out tom 
	}
}






#define RAYGUI_IMPLEMENTATION
#include "include/drawing.h"
#include "include/entities.h"
#include "include/physics.h"
#include "include/quadtree.h"
#include <raylib.h>


void create_sun(Entities* e)
{
	e->pos[0] = (Vector2){(SIM_MAX_WIDTH_COORD+SIM_MIN_WIDTH_COORD) / 2., (SIM_MAX_HEIGHT_COORD+SIM_MIN_HEIGHT_COORD)/2.};
	e->vel[0] = (Vector2){0,0};
	e->acc[0] = (Vector2){0,0};
	e->r[0] = 25;
	e->m[0] = 1000;
	e->color[0] = YELLOW;
}

/*
 *
 *
 * Things i want control of:
 * particle count
 * gravity
 * timestep
 *
 *
 * 
 *
 *
 *
 * */

int main(/* int argc, char** argv */){
	InitWindow(1280, 1000, "Particle Simulation [ NAIVE ]");
	if(!IsWindowReady())
	{
		return 1;
	}
	// srand(time(NULL));
	srand(0);


	Options opts = {DEFAULT_PARTICLES,DEFAULT_GRAVITY, DEFAULT_DT};

	Entities entities = {.nents = DEFAULT_PARTICLES};
	int err = alloc_rand_entities(&entities);
	if(err)
	{
		check_free(entities);
		CloseWindow();
		return 1;
	}
	create_sun(&entities);

	err = init_node_pool(entities.nents);
	if(err)
	{
		check_free(entities);
		CloseWindow();
		return 1;
	}
	
	double frametime_start;
	double render_start;
	double update_start;
	double frametime_end;
	double render_end;
	double update_end;
	bool running = true;
	SetTargetFPS(FPS);

	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{
		frametime_start = GetTime();
		
		if(IsKeyPressed(KEY_R))
		{
			opts.gravity = DEFAULT_GRAVITY;
		}

		Options sopts = opts;
		if((size_t)sopts.nparticles > entities.nents)
		{
			err = realloc_rand_nentities(&entities,(int)sopts.nparticles);
			if(err)
			{
				break;
			}
			err = realloc_node_pool((size_t)sopts.nparticles);
		}

		if((size_t)sopts.nparticles != entities.nents)
		{
			entities.nents = (size_t)sopts.nparticles;	
		}

		BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangleLines(SIM_MIN_WIDTH_COORD,SIM_MIN_HEIGHT_COORD,SIM_DIM,SIM_DIM,WHITE);
		
		if(running)
		{
			render_start = GetTime();
			draw_entities(entities);
			render_end = GetTime();
			// printf("Made it past draw_entities\n");

			update_start = GetTime();
			update_entities(&entities, sopts);
			update_end = GetTime();
			// printf("Made it past update_entiites\n");

		}
		else {
			render_start = GetTime();
			render_end = GetTime();
			update_start = GetTime();
			update_end = GetTime();
		}
				
		DrawFPS(0, 0);

		frametime_end = GetTime();
		
		draw_diagnostics(frametime_start, frametime_end, render_start, render_end, update_start, update_end);
		draw_gui(&opts);
		EndDrawing();

	}

	free_node_pool();
	check_free(entities);
	CloseWindow();
	return 0;
}

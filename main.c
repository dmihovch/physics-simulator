#define RAYGUI_IMPLEMENTATION
#include "include/drawing.h"
#include "include/entities.h"
#include "include/physics.h"
#include <raylib.h>
#ifndef RAYLIB
#define RAYLIB
#endif //RAYLIB

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

	Entities entities = {.n_ents = DEFAULT_PARTICLES};
	int err = alloc_rand_entities(&entities);
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
		if((size_t)sopts.nparticles > entities.n_ents)
		{
			err = realloc_rand_nentities(&entities,(int)sopts.nparticles);
			if(err)
			{
				break;
			}
		}

		if((size_t)sopts.nparticles != entities.n_ents)
		{
			entities.n_ents = (size_t)sopts.nparticles;	
		}

	
			// realloc_nparticles(p, particles_count, particles_count+5);
			// might handle this more gracefully later, but for now we can just not render and compute the particles that are 'removed'

		BeginDrawing();
		ClearBackground(BLACK);
		if(running)
		{
			render_start = GetTime();
			draw_entities(entities);
			render_end = GetTime();

			update_start = GetTime();
			update_entities(&entities, sopts);
			update_end = GetTime();

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


	check_free(entities);
	CloseWindow();
	return 0;
}

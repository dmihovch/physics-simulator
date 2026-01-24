#define RAYGUI_IMPLEMENTATION
#include "include/drawing.h"
#include "include/particle.h"
#include "include/physics.h"
#include <raylib.h>

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
	InitWindow(WIDTH, HEIGHT, "Particle Simulation [ NAIVE ]");
	if(!IsWindowReady())
	{
		return 1;
	}
	srand(time(NULL));

	Options opts = {10.,DEFAULT_GRAVITY, DEFAULT_DT};

	Particle* particles = alloc_rand_nparticles((int)opts.nparticles);
	if(particles == NULL){
		CloseWindow();
		return 1;
	}
	
	particles[0] = (Particle)
	{
		.pos = {WIDTH/2.,HEIGHT/2.},
		.vel = {0,0},
		.acc = {0,0},
		.r = 10.,
		.m = 1000.,
		.color = YELLOW
	};

	double frametime_start;
	double render_start;
	double update_start;
	double frametime_end;
	double render_end;
	double update_end;
	int rendered_particles = opts.nparticles;
	bool running = true;
	SetTargetFPS(FPS);
	int err;
	while(!WindowShouldClose() && GetKeyPressed() != KEY_Q)
	{
		printf("start loop\n");
		frametime_start = GetTime();
		Options sopts = opts;
		printf("in main: %d\t%d\n",(int)sopts.nparticles,rendered_particles);
		if((int)sopts.nparticles > rendered_particles)
		{
			printf("%d\t%d\n",(int)sopts.nparticles,rendered_particles);
			particles = realloc_rand_nparticles(particles,(int)sopts.nparticles,rendered_particles);
			if(particles == NULL)
			{
				break;
			}
		}

		if((int)sopts.nparticles != rendered_particles)
		{
			rendered_particles = (int)sopts.nparticles;	
		}

	
			// realloc_nparticles(p, particles_count, particles_count+5);
			// might handle this more gracefully later, but for now we can just not render and compute the particles that are 'removed'

		BeginDrawing();
		ClearBackground(BLACK);
		if(running)
		{
			render_start = GetTime();
			draw_particles(particles, (int)sopts.nparticles);
			render_end = GetTime();

			update_start = GetTime();
			update_particles(particles, sopts);
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
		printf("here again\n");
		EndDrawing();
		printf("end loop\n");

	}


	free(particles);
	CloseWindow();
	return 0;
}

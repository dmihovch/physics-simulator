#ifndef DRAWING_H
#define DRAWING_H
#include "particle.h"
#include "physics.h"
#include "raygui.h"
#include <string.h>
#include <stdio.h>

void draw_particles(Particle* p, int n);
int draw_diagnostics(double frametime_start, double frametime_end, double render_start, double render_end, double update_start, double update_end);
void draw_gui(Options* opts);
#endif //DRAWING_H

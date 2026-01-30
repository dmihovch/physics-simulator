#include "../include/entities.h"

int alloc_rand_entities(Entities* e)
{

	e->pos = malloc(e->nents*sizeof(Vector2));
	e->vel = malloc(e->nents*sizeof(Vector2));
	e->acc = malloc(e->nents*sizeof(Vector2));
	e->r = malloc(e->nents*sizeof(float));
	e->m = malloc(e->nents*sizeof(float));
	e->color = malloc(e->nents*sizeof(Color));
	e->gridpos = malloc(e->nents*(sizeof(size_t)));
	e->oldpos = malloc(e->nents*sizeof(Vector2));

	if(e->pos == NULL ||
	   e->vel == NULL ||
	   e->acc == NULL ||
	   e->r == NULL ||
	   e->m == NULL ||
	   e->color == NULL ||
	   e->gridpos == NULL ||
	   e->oldpos == NULL
	)
	{
		return 1;
	}

	for(size_t i = 0; i < e->nents; ++i)
	{
		create_rand_entity(e,i);
	}

	return 0;

}

void create_rand_entity(Entities* e, size_t i)
{
	e->pos[i] = (Vector2){rand_float(SIM_MIN_WIDTH_COORD,SIM_MAX_WIDTH_COORD),rand_float(SIM_MIN_HEIGHT_COORD,SIM_MAX_HEIGHT_COORD)};
	e->vel[i] = (Vector2){rand_float(-20.0,20.0),rand_float(-20.0,20.0)};
	e->acc[i] = (Vector2){0,0};
	e->r[i] = 5.;
	e->m[i] = .001;
	e->color[i] = rand_color();
	e->gridpos[i] = (IVec2){-1,-1};
}

int check_free(Entities e)
{
	int non_null = 0;
	if(e.pos != NULL)
	{
		non_null = 1;
		free(e.pos);
	}
	if(e.vel != NULL)
	{
		non_null = 1;
		free(e.vel);
	}
	if(e.acc != NULL)
	{
		non_null = 1;
		free(e.acc);
	}
	if(e.r != NULL)
	{
		non_null = 1;
		free(e.r);
	}
	if(e.m != NULL)
	{
		non_null = 1;
		free(e.m);
	}
	if(e.color != NULL)
	{
		non_null = 1;
		free(e.color);
	}
	if(e.gridpos != NULL)
	{
		non_null = 1;
		free(e.gridpos);
	}
	if(e.oldpos != NULL)
	{
		non_null = 1;
		free(e.oldpos);
	}
	return non_null;
}

int realloc_rand_nentities(Entities* e, size_t new_nents)
{
	size_t vec2_nents = sizeof(Vector2) * new_nents;
	size_t float_nents = sizeof(float) * new_nents;

	Entities tmp = {0};
	tmp.nents = new_nents;
	tmp.pos = realloc(e->pos, vec2_nents);
	if(tmp.pos != NULL)
	{
		e->pos = tmp.pos;
	}
	tmp.vel = realloc(e->vel,vec2_nents);
	if(tmp.vel != NULL)
	{
		e->vel = tmp.vel;
	}
	tmp.acc = realloc(e->acc, vec2_nents);
	if(tmp.acc != NULL)
	{
		e->acc = tmp.acc;
	}
	tmp.r = realloc(e->r, float_nents);
	if(tmp.r != NULL)
	{
		e->r = tmp.r;
	}
	tmp.m = realloc(e->m, float_nents);
	if(tmp.m != NULL)
	{
		e->m = tmp.m;
	}
	tmp.color = realloc(e->color, new_nents * sizeof(Color));
	if(tmp.color != NULL)
	{
		e->color = tmp.color;
	}
	tmp.gridpos = realloc(e->gridpos, new_nents * sizeof(IVec2));
	if(tmp.gridpos != NULL)
	{
		e->gridpos = tmp.gridpos;
	}
	tmp.oldpos = realloc(e->oldpos, vec2_nents);

	if(
		tmp.pos == NULL ||
		tmp.vel == NULL ||
		tmp.acc == NULL ||
		tmp.r == NULL ||
		tmp.m == NULL ||
		tmp.color == NULL ||
		tmp.gridpos == NULL ||
		tmp.oldpos == NULL
	)
	{
		check_free(tmp);
		return 1;
	}

	size_t old_nents = e->nents;

	for(size_t i = old_nents; i<new_nents; ++i)
	{
		create_rand_entity(e,i);
	}

	return 0;
}


Color rand_color()
{
	Color colors[] = {RAYWHITE,RED,BLUE,PURPLE,PINK,YELLOW,ORANGE,GREEN};
	return colors[rand()%8];
}


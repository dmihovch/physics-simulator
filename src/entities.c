#include "../include/entities.h"

int alloc_rand_entities(Entities* e)
{

	e->pos = malloc(e->n_ents*sizeof(Vector2));
	e->vel = malloc(e->n_ents*sizeof(Vector2));
	e->acc = malloc(e->n_ents*sizeof(Vector2));
	e->r = malloc(e->n_ents*sizeof(float));
	e->m = malloc(e->n_ents*sizeof(float));
	e->color = malloc(e->n_ents*sizeof(Color));

	if(e->pos == NULL ||
	   e->vel == NULL ||
	   e->acc == NULL ||
	   e->r == NULL ||
	   e->m == NULL ||
	   e->color == NULL
	)
	{
		return 1;
	}

	for(size_t i = 0; i < e->n_ents; ++i)
	{
		create_rand_entity(e,i);
	}

	return 0;

}

void create_rand_entity(Entities* e, size_t i)
{
	e->pos[i] = (Vector2){rand_float(0,SIM_WIDTH),rand_float(0,SIM_HEIGHT)};
	e->vel[i] = (Vector2){rand_float(-20.0,20.0),rand_float(-20.0,20.0)};
	e->acc[i] = (Vector2){0,0};
	e->r[i] = 5.;
	e->m[i] = .001;
	e->color[i] = rand_color();
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
	return non_null;
}

int realloc_rand_nentities(Entities* e, size_t new_nents)
{
	size_t vec2_nents = sizeof(Vector2) * new_nents;
	size_t float_nents = sizeof(float) * new_nents;

	Entities tmp = {0};
	tmp.n_ents = new_nents;
	tmp.pos = realloc(e->pos, vec2_nents);
	tmp.vel = realloc(e->vel,vec2_nents);
	tmp.acc = realloc(e->acc, vec2_nents);
	tmp.r = realloc(e->r, float_nents);
	tmp.m = realloc(e->m, float_nents);
	tmp.color = realloc(e->color, new_nents * sizeof(Color));

	if(
		tmp.pos == NULL ||
		tmp.vel == NULL ||
		tmp.acc == NULL ||
		tmp.r == NULL ||
		tmp.m == NULL ||
		tmp.color == NULL
	)
	{
		check_free(tmp);
		return 1;
	}

	size_t old_nents = e->n_ents;

	e->n_ents = tmp.n_ents;
	e->pos = tmp.pos;
	e->vel = tmp.vel;
	e->acc = tmp.acc;
	e->r = tmp.r;
	e->m = tmp.m;
	e->color = tmp.color;

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


#include "../include/quadtree.h"

#define NE 1
#define NW 2
#define SE 3
#define SW 4

static QNode* node_pool;
static size_t node_pool_len = DEFAULT_PARTICLES*NODES_MULT;
static size_t next_free_node = 0;

int init_node_pool()
{
	node_pool = malloc(node_pool_len*sizeof(QNode));
	if(node_pool == NULL)
	{
		return 1;
	}
	return 0;
}

QNode* get_next_node()
{
	if(next_free_node >= node_pool_len)
	{
		printf("too many nodes\n");
		return NULL;
	}
	QNode* node = &node_pool[next_free_node];
	node->com = (Vector2){0.0f,0.0f};
	node->cum_mass = 0.0f;
	node->entity = LEAF;
	for(int i = 0; i<4; ++i)
	{
		node->quads[i] = NULL;
	}
	next_free_node++;
	return node;
}


int build_quadtree(Entities* e)
{
	next_free_node = 0;	

	QNode* root = get_next_node();
	root->com = e->pos[0];
	root->cum_mass = e->m[0];
	root->entity = 0;
	root->quad = 
		(Quadrant)
		{
			.cx = (SIM_MAX_WIDTH_COORD+SIM_MIN_WIDTH_COORD)/2,
			.cy = (SIM_MAX_HEIGHT_COORD+SIM_MIN_HEIGHT_COORD)/2,
			.half = SIM_DIM/2,
		};

	QNode* node;
	for(size_t i = 1; i<e->nents; ++i)
	{
			insert_qentity(e->root,e,i);
	}

	return 0;
	
}

int get_quadrant(QNode* node, Vector2 pos)
{
	bool allnull = true;
	for(int i = 0; i<4; i++)
	{
		if(node->quads[i] != NULL)
		{
			allnull = false;
			break;
		}
	}
	if(allnull)
	{
		return LEAF;
	}


	//get the actual quadrant

}

void insert_qentity(QNode* node, Entities* e, size_t i)
{

	vec2_add_ip(&node->com,vec2_scalar_mult(e->pos[i],e->m[i]));
	node->cum_mass += e->m[i];
	if(node->entity != -1)
	{
		//subdivide
		return;
	}
	if(node->entity == -1 )
	{

		int quad = get_quadrant(node,e->pos[i]);
		if(quad == LEAF)
		{
			//leaf, copy data
			return;
		}

		insert_qentity(node->quads[quad],e,i);
		return;
	}

}






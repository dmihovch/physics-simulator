#include "../include/quadtree.h"


static QNode* node_pool;
static size_t node_pool_len = 0;
static size_t next_free_node = 0;

void free_node_pool(void)
{
	if(node_pool != NULL)
	{
		free(node_pool);
	}
}

int init_node_pool(size_t nents)
{
	node_pool_len = nents*NODES_MULT;
	node_pool = malloc(node_pool_len*sizeof(QNode));
	if(node_pool == NULL)
	{
		return 1;
	}
	return 0;
}

QNode* get_next_node(Quadrant parent, enum Direction dir)
{
	if(next_free_node >= node_pool_len)
	{
		printf("too many nodes\n");
		return NULL;
	}
	QNode* node = &node_pool[next_free_node];
	if(node == NULL)
	{
		return NULL;
	}
	node->com = (Vector2){0.0f,0.0f};
	node->cum_mass = 0.0f;
	node->entity = -1;

	switch(dir)
	{
		case NE:
			node->quad = (Quadrant){
				.cx = parent.cx + parent.half/2,
				.cy = parent.cy - parent.half/2,
				.half = parent.half/2
			};
			break;
		
		case NW:
			node->quad = (Quadrant){
				.cx = parent.cx - parent.half/2,
				.cy = parent.cy - parent.half/2,
				.half = parent.half/2
			};
			break;

		case SE:
			node->quad = (Quadrant){
				.cx = parent.cx + parent.half/2,
				.cy = parent.cy + parent.half/2,
				.half = parent.half/2
			};
			break;
		
		case SW:
			node->quad = (Quadrant){
				.cx = parent.cx - parent.half/2,
				.cy = parent.cy + parent.half/2,
				.half = parent.half/2
			};
			break;

		default:
			node->quad = parent;
			break;
	}
	

	for(int i = 0; i<4; ++i)
	{
		node->quads[i] = NULL;
	}
	next_free_node++;
	return node;
}


int build_quadtree(Entities* e)
{
	printf("in build_quad\n");
	next_free_node = 0;	

	e->root = get_next_node((Quadrant)
		{
			.cx = (SIM_MAX_WIDTH_COORD+SIM_MIN_WIDTH_COORD)/2,
			.cy = (SIM_MAX_HEIGHT_COORD+SIM_MIN_HEIGHT_COORD)/2,
			.half = SIM_DIM/2,
		}, -1);
	if(e->root == NULL)
	{
		printf("root is null\n");
		return 1;
	}
	printf("got next node\n");
	e->root->com = e->pos[0];
	e->root->cum_mass = e->m[0];
	e->root->entity = 0;
	for(size_t i = 1; i<e->nents; ++i)
	{
			printf("inserting %ld... ",i);
			insert_qentity(e->root,e,i);
			printf("done!\n");
	}

	return 0;
	
}

enum Direction get_quadrant(QNode* node, Vector2 pos)
{
	Quadrant quad = node->quad;
	bool positive_x = (pos.x >= quad.cx);
	bool positive_y = (pos.y >= quad.cy);
	if(positive_x) //East
	{
		if(positive_y)	
		{
			return SE;
		}
		return NE;
	}
	//West
	if(positive_y)
	{
		return SW;
	}
	return NW;
}

bool is_empty_leaf(QNode* node)
{
	for(int i = 0; i<4; ++i)
	{
		if(node->quads[i] != NULL)
		{
			return false;
		}
	}
	return true;
}

void insert_qentity(QNode* node, Entities* e, size_t i)
{
	if(node == NULL)
	{
		return;
	}
	vec2_add_ip(&node->com,vec2_scalar_mult(e->pos[i],e->m[i]));
	node->cum_mass += e->m[i];
	if(node->entity != -1) //subdivide
	{
		node->quads[NE] = get_next_node(node->quad,NE);
		node->quads[NW] = get_next_node(node->quad,NW);
		node->quads[SE] = get_next_node(node->quad,SE);
		node->quads[SW] = get_next_node(node->quad,SW);

		enum Direction new_par_dir = get_quadrant(node, e->pos[node->entity]);
		enum Direction new_node_dir = get_quadrant(node, e->pos[i]);
		
		insert_qentity(node->quads[new_par_dir],e,node->entity);
		insert_qentity(node->quads[new_node_dir],e,i);
		node->entity = -1;
		return;
	}


	//internal or leaf
	if(node->entity == -1 )
	{
		if(is_empty_leaf(node))
		{
			node->com = e->pos[i];
			node->cum_mass = e->m[i];
			node->entity = i;
			return;
		}

		enum Direction quad = get_quadrant(node,e->pos[i]);
		insert_qentity(node->quads[quad],e,i);
		return;
	}

}






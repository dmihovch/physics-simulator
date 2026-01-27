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

int realloc_node_pool(size_t nents)
{
	if(node_pool == NULL)
	{
		return 1;
	}
	
	free(node_pool);
	return init_node_pool(nents);
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
	memset(node,0,sizeof(QNode));
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
	next_free_node++;
	return node;
}


QNode* build_quadtree(Entities* e)
{
	// printf("in build_quad\n");
	next_free_node = 0;	
	QNode* root;
	root = get_next_node((Quadrant)
		{
			.cx = (SIM_MAX_WIDTH_COORD+SIM_MIN_WIDTH_COORD)/2,
			.cy = (SIM_MAX_HEIGHT_COORD+SIM_MIN_HEIGHT_COORD)/2,
			.half = SIM_DIM/2,
		}, -1);
	if(root == NULL)
	{
		printf("root is null\n");
		return NULL;
	}
	// printf("got next node\n");
	for(size_t i = 0; i<e->nents; ++i)
	{
			// printf("inserting %ld... ",i);
			insert_qentity(root,e,i);
			// printf("done!\n");
	}

	return root;
	
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
	if(node->entity != -1 && node->quad.half > TREE_DEPTH_LIMITER) //subdivide
	{
		enum Direction new_par_dir = get_quadrant(node, e->pos[node->entity]);
		enum Direction new_node_dir = get_quadrant(node, e->pos[i]);
		if(node->quads[new_par_dir] == NULL)
		{
			node->quads[new_par_dir] = get_next_node(node->quad,new_par_dir);
		}
		if(node->quads[new_node_dir] == NULL)
		{
			node->quads[new_node_dir] = get_next_node(node->quad, new_node_dir);
		}
		
		insert_qentity(node->quads[new_par_dir],e,node->entity);
		insert_qentity(node->quads[new_node_dir],e,i);
		node->entity = -1;
		return;
	}
	if(node->entity != -1 && node->quad.half <= TREE_DEPTH_LIMITER)
	{
		enum Direction new_shared_dir = get_quadrant(node,e->pos[node->entity]);
		if(node->quads[new_shared_dir] == NULL)
		{
			node->quads[new_shared_dir] = get_next_node(node->quad,new_shared_dir);
			node->quads[new_shared_dir]->cum_mass += e->m[i];
			insert_qentity(node->quads[new_shared_dir],e,node->entity);
		}
		node->entity = -1;
		return;
	}


	//internal or leaf
	if(node->entity == -1 )
	{
		if(is_empty_leaf(node))
		{
			node->entity = i;
			return;
		}

		enum Direction quad = get_quadrant(node,e->pos[i]);
		if(node->quads[quad] == NULL)
		{
			node->quads[quad] = get_next_node(node->quad,quad);
		}
		insert_qentity(node->quads[quad],e,i);
		return;
	}

}






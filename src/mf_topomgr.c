#include "mf_topomgr.h"
#include "mf_switch.h"
#include "dbg.h"
#include "mf_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static struct mf_topomgr MF_TOPO_MGR;

static struct link_node * LINK_NODE_CACHE_ARRAY; 

static struct network_link NETWORK_LINK_CACHE_ARRAY[MAX_NETWORK_LINK_NUM];

static inline void push_to_array(struct link_node * value, struct link_node ** array);

static inline struct link_node* pop_from_array(struct link_node * value, struct link_node ** array);

void mf_topomgr_create()
{
	MF_TOPO_MGR.total_node_number = 0;
	MF_TOPO_MGR.total_network_link_number = 0;
	MF_TOPO_MGR.node_cache_array_size = LINK_NODE_NUM;
	pthread_mutex_init(&(MF_TOPO_MGR.topomgr_mutex), NULL);
	LINK_NODE_CACHE_ARRAY = (struct link_node *)malloc(MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	if(LINK_NODE_CACHE_ARRAY == NULL)
	{
		log_err("topo mgr malloc failed\n");
		exit(0);
	}
	memset(LINK_NODE_CACHE_ARRAY, 0 , MF_TOPO_MGR.node_cache_array_size * sizeof(*LINK_NODE_CACHE_ARRAY));
	int i = 0;
	/* Network Node list*/
	MF_TOPO_MGR.available_list.next = NULL;
	MF_TOPO_MGR.available_list.mark= 0;
	for(i = 0; i< MF_TOPO_MGR.node_cache_array_size; i++)
		lf_list_insert(&(LINK_NODE_CACHE_ARRAY[i].mem_manage_list), &(MF_TOPO_MGR.available_list));
	MF_TOPO_MGR.used_list.next = NULL;
	MF_TOPO_MGR.used_list.mark= 0;
	/*Network link list*/
	MF_TOPO_MGR.available_link_list.next = NULL;
	MF_TOPO_MGR.available_link_list.mark= 0;
	for(i = 0; i< MAX_NETWORK_LINK_NUM; i++)
		lf_list_insert(&(NETWORK_LINK_CACHE_ARRAY[i].mem_manage_list), &(MF_TOPO_MGR.available_link_list));
	MF_TOPO_MGR.used_link_list.next= NULL;
	MF_TOPO_MGR.used_link_list.mark= 0;
}

void mf_topomgr_destroy()
{
	if(LINK_NODE_CACHE_ARRAY)  
		free(LINK_NODE_CACHE_ARRAY);
	pthread_mutex_destroy(&(MF_TOPO_MGR.topomgr_mutex));
	log_info("TOPO manager destroyed");
}

static void realloc_cache_array()
{
	pthread_mutex_lock(&MF_TOPO_MGR.topomgr_mutex);
	LINK_NODE_CACHE_ARRAY = (struct link_node *)realloc(LINK_NODE_CACHE_ARRAY, 2 * MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	if(LINK_NODE_CACHE_ARRAY == NULL)
	{
		log_err("realloc cache array failed");
		exit(0);
	}
	memset(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size, 0, MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
	int i;
	for(i = MF_TOPO_MGR.node_cache_array_size; i< MF_TOPO_MGR.node_cache_array_size * 2; i++)
		lf_list_insert(&(LINK_NODE_CACHE_ARRAY[i].mem_manage_list), &(MF_TOPO_MGR.available_list));
	MF_TOPO_MGR.node_cache_array_size *= 2;
}

static struct link_node * get_available_value_slot()
{
	if(MF_TOPO_MGR.available_list.next == NULL)
		realloc_cache_array();
	struct lf_list* l = lf_list_pop(&MF_TOPO_MGR.available_list);
	struct link_node * value = container_of(l, struct link_node, mem_manage_list);
	lf_list_insert(l, &(MF_TOPO_MGR.used_list));
	return value;
}

struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port)
{
	if(port->node != NULL)
	{
		//log_warn("Node already exists");
		return (port->node);
	}
	struct link_node * node = get_available_value_slot();
	if(unlikely(node == NULL))
	{
		log_warn("Bad value slot");
		return NULL;
	}
	port->node= node;
	node->sw = sw;
	node->port = port;
	node->next = NULL;
	node->prev = NULL;
	node->is_occupied = 1;
	node->mem_manage_list.mark= 0;//Not necessary actually...But...Keep it as a good hobby..
	MF_TOPO_MGR.total_node_number++;
	return node;
}


struct network_link * network_link_create(struct link_node* src, struct link_node* dst)
{
	if(src->port->link != NULL)
	{
		return NULL;
	}
	struct lf_list * l = lf_list_pop(&(MF_TOPO_MGR.available_link_list));
	if(l == NULL)
	{
		log_warn("No available slot");
		return NULL;
	}
	struct network_link * link = container_of(l, struct network_link, mem_manage_list);
	lf_list_insert(l, &(MF_TOPO_MGR.used_link_list)); 
	MF_TOPO_MGR.total_network_link_number++;
	link->src = src;
	src->port->link = link;
	link->dst = dst;
	link->sw_next.next = NULL;
	link->sw_next.mark = 0;
	lf_list_insert(l, &(MF_TOPO_MGR.used_link_list));
	return link;
}

struct path_link_list * path_link_list_create()
{
	struct path_link_list * list = (struct path_link_list *)malloc(sizeof(*list));
	list->hop_num = 0;
	memset(&(list->path_link_list[0]), 0, sizeof(LONGEST_PATH_LINK_NUM * sizeof(struct network_link *)));
	return list;
}

uint32_t sw_link_insert(struct sw_link_list * sw_list, struct network_link * link)
{
	if(unlikely(link == NULL))
	{
		log_warn("link is NULL");
		return 0;
	}
	pthread_mutex_lock(&(link->src->sw->switch_mutex));
	if(sw_list->link_num == 0 && sw_list->head == NULL)
	{
		sw_list->head = link;
	}
	else
	{
		link->sw_link_next = sw_list->head;
		sw_list->head = link;
	}
	log_info("New network link discovered");
	sw_list->link_num++;
	pthread_mutex_unlock(&(link->src->sw->switch_mutex));
	return 1;
}

void network_path_insert(struct path_link_list * list, struct network_link * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->hop_num == 0 && list->path_link_list[0] == NULL)
	{
		list->path_link_list[0] = link;
	}
	else
	{
		list->path_link_list[list->hop_num + 1] = link;
	}
	list->hop_num++;
}

void sw_link_delete(struct sw_link_list * list, struct network_link * link)
{
	if(list == NULL || list->head == NULL)
		return;
	struct network_link * tmp = list->head;
	struct network_link * curr = NULL;
	while(tmp)
	{
		if(tmp == link)
		{
			if(tmp == list->head)
			{
				list->head = tmp->sw_link_next;
				list->link_num--;
				network_link_free(tmp);
				break;
			}
			else
			{
				curr->sw_link_next = tmp->sw_link_next;
				list->link_num--;
				network_link_free(tmp);
				break;
			}
		}
		curr = tmp;
		tmp = tmp->sw_link_next;
	}
}

void path_link_list_free(struct path_link_list * list)
{
	if(list)
		free(list);
}

void network_link_free(struct network_link * link)
{
	if(link->is_occupied == 1)
		link->is_occupied = 0;
}

/*code draft*/
/*
struct path_link_list * find_one_path_between_switches(struct mf_switch * src_sw, struct mf_switch * dst_sw)
{
	struct path_link_list * path = path_link_list_create();
	int i = 0;
	char depth = 0;
	char search_depth[LONGEST_PATH_LINK_NUM] ;
	struct network_link * tmp = src_sw->link_list.head;
	for(;i <= src_sw->link_list.link_num; i++)
	{
		if(tmp->dst->sw == dst_sw)
		{
			path->path_link_list[depth] = tmp;
			return path;
		}
		else if(tmp == NULL)
		{
			depth--;
 			tmp =  
		}
		else
		{
  			depth++;
			tmp = 
		}
	}
	log_warn("Can not find a link between switch dpid:%ld and switch dpid:%ld", src_sw->datapath_id, dst_sw->datapath_id);
	return NULL;
}
*/

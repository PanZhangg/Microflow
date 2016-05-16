#include "mf_topomgr.h"
#include "mf_switch.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static struct mf_topomgr MF_TOPO_MGR;

static struct link_node * LINK_NODE_CACHE_ARRAY; 

static struct network_link NETWORK_LINK_CACHE_ARRAY[MAX_NETWORK_LINK_NUM];

static void push_to_array(struct link_node * value, struct link_node ** array);

void mf_topomgr_create()
{
	MF_TOPO_MGR.total_node_number = 0;
	MF_TOPO_MGR.node_cache_array_size = 1024;
	pthread_mutex_init(&(MF_TOPO_MGR.topomgr_mutex), NULL);
	LINK_NODE_CACHE_ARRAY = (struct link_node *)malloc(MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	memset(LINK_NODE_CACHE_ARRAY, 0 , sizeof(*LINK_NODE_CACHE_ARRAY));
	if(LINK_NODE_CACHE_ARRAY == NULL)
	{
		printf("topo mgr malloc failed\n");
		exit(0);
	}
	int i = 0;
	for(; i< MF_TOPO_MGR.node_cache_array_size; i++)
		push_to_array(LINK_NODE_CACHE_ARRAY + i, &(MF_TOPO_MGR.available_slot));
	MF_TOPO_MGR.used_slot = NULL;
	MF_TOPO_MGR.next_available_index = 0;
}

static inline void push_to_array(struct link_node * value, struct link_node ** array)
{
	if(*array == NULL)
	{
		*array = value;
		value->prev = NULL;
		value->next = NULL;
	}
	else
	{
		(*array)->prev = value;
		value->next = *array;
		value->prev = NULL;
		*array = value;
	}
}

static inline struct link_node* pop_from_array(struct link_node * value, struct link_node ** array)
{
	if(array == NULL || *array == NULL)
		return NULL;
	struct link_node * tmp = * array;
	while(tmp)
	{
		if(tmp == value)
		{
			if(tmp->prev == NULL)
			{	
				if(tmp->next != NULL)
				{
					* array = tmp->next;
					tmp->next->prev = NULL;
				}
				else
				{
					* array = NULL;
				}
				tmp->next = NULL;
				return tmp;
			}
			if(tmp->next == NULL)
			{
				tmp->prev->next = NULL;
				tmp->prev = NULL;
				return tmp;
			}
			if(tmp->prev && tmp->next)
			{
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
				return tmp;
			}
		}
		else
			if(tmp->next)
				tmp = tmp->next;
			else
				return NULL;
	}
	return NULL;
}

static void realloc_cache_array()
{
	LINK_NODE_CACHE_ARRAY = (struct link_node *)realloc(LINK_NODE_CACHE_ARRAY, 2 * MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	if(LINK_NODE_CACHE_ARRAY == NULL)
	{
		printf("realloc cache array failed\n");
		exit(0);
	}
	memset(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size, 0, MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	int i;
	for(i = MF_TOPO_MGR.node_cache_array_size; i< MF_TOPO_MGR.node_cache_array_size * 2; i++)
		push_to_array(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size + i, &(MF_TOPO_MGR.available_slot));
	MF_TOPO_MGR.node_cache_array_size *= 2;
}

static struct link_node * get_available_value_slot()
{
	if(MF_TOPO_MGR.available_slot == NULL)
		realloc_cache_array();
	if(MF_TOPO_MGR.available_slot->is_occupied == 0)
	{
		struct link_node * value = pop_from_array(MF_TOPO_MGR.available_slot, &(MF_TOPO_MGR.available_slot));
		return value;
	}
	return NULL;
}

struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port)
{
	struct link_node * node = get_available_value_slot();
	node->sw = sw;
	node->port = port;
	node->next = NULL;
	node->prev = NULL;
	node->is_occupied = 1;
	push_to_array(node, &(MF_TOPO_MGR.used_slot));
	return node;
}

static uint32_t get_next_available_index()
{
	static uint8_t loop_restart;
	uint32_t index = MF_TOPO_MGR.next_available_index;
	if(NETWORK_LINK_CACHE_ARRAY[index].is_occupied == 0)
		return index;
	if(++index > MAX_NETWORK_LINK_NUM)
	{
		loop_restart = 1;
		index = 0;
	}
	while(NETWORK_LINK_CACHE_ARRAY[index].is_occupied == 1)
	{	
		index++;
		if(index > MAX_NETWORK_LINK_NUM && \
			loop_restart == 1)
		{
			loop_restart = 0;
			printf("no network link slot is available\n");
			exit(0);
		}
		else if(index > MAX_NETWORK_LINK_NUM && \
			loop_restart == 0)
		{
			loop_restart = 1;
			MF_TOPO_MGR.next_available_index = 0;
		}
	}
	return index;
}

struct network_link * network_link_create(struct link_node* src, struct link_node* dst)
{
	uint32_t index = get_next_available_index();
	NETWORK_LINK_CACHE_ARRAY[index].src = src;
	NETWORK_LINK_CACHE_ARRAY[index].dst = dst;
	NETWORK_LINK_CACHE_ARRAY[index].is_occupied = 1;
	NETWORK_LINK_CACHE_ARRAY[index].sw_link_next = NULL;
	return (& NETWORK_LINK_CACHE_ARRAY[index]);
}

struct path_link_list * path_link_list_create()
{
	struct path_link_list * list = (struct path_link_list *)malloc(sizeof(*list));
	list->hop_num = 0;
	memset(&(list->path_link_list[0]), 0, sizeof(LONGEST_PATH_LINK_NUM * sizeof(struct network_link *)));
	return list;
}

void sw_link_insert(struct sw_link_list * list, struct network_link * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->link_num == 0 && list->head == NULL)
	{
		list->head = link;
	}
	else
	{
		link->sw_link_next = list->head;
		list->head = link;
	}
	list->link_num++;
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
struct path_link_list * find_one_path_between_switches(struct mf_switch * src_sw, struct mf_switch * dst_sw)
{

	struct path_link_list * path = path_link_list_create();
	int i = 0;
	struct network_link * tmp = src_sw->link_list.head;
	/*for(;i <= src_sw->link_list.link_num; i++)
	{
		if(tmp->dst->sw == dst_sw)


	}*/
}
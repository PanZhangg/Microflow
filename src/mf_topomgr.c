#include "mf_topomgr.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static struct mf_topomgr MF_TOPO_MGR;

static struct link_node * LINK_NODE_CACHE_ARRAY; 

//static struct network_link NETWORK_LINK_CACHE_ARRAY[MAX_NETWORK_LINK_NUM];

//static struct network_link NETWORK_LINK_CACHE_ARRAY[];
static void push_to_array(struct link_node * value, struct link_node ** array);

void mf_topomgr_create()
{
	MF_TOPO_MGR.total_node_number = 0;
	MF_TOPO_MGR.node_cache_array_size = 256;
	pthread_mutex_init(&(MF_TOPO_MGR.devicemgr_mutex), NULL);
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
}

static void push_to_array(struct link_node * value, struct link_node ** array)
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

static struct link_node* pop_from_array(struct link_node * value, struct link_node ** array)
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
	memset(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size, 0, sizeof(MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node)));
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

struct network_link * network_link_create(struct link_node* src, struct link_node* dst)
{
	struct network_link * link = (struct network_link *)malloc(sizeof(*link));
	link->src = src;
	link->dst = dst;
	return link;
}

struct link_list_element * link_list_element_create(struct network_link * link)
{
	struct link_list_element * link_element = (struct link_list_element *)malloc(sizeof(*link));
	link_element->link = link;
	link_element->next = NULL;
	return link_element;
}

struct sw_link_list * sw_link_list_create()
{
	struct sw_link_list * list = (struct sw_link_list *)malloc(sizeof(*list));
	list->link_num = 0;
	list->head = NULL;
	return list;
}

struct path_link_list * path_link_list_create()
{
	struct path_link_list * list = (struct path_link_list *)malloc(sizeof(*list));
	list->hop_num = 0;
	list->head = NULL;
	return list;
}

void sw_link_insert(struct sw_link_list * list, struct link_list_element * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->link_num == 0 && list->head == NULL)
	{
		list->head = link;
	}
	else
	{
		struct link_list_element * tmp = list->head;
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = link;
	}
	list->link_num++;
}

void network_path_insert(struct path_link_list * list, struct link_list_element * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->hop_num == 0 && list->head == NULL)
	{
		list->head = link;
	}
	else
	{
		struct link_list_element * tmp = list->head;
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = link;
	}
	list->hop_num++;
}

void sw_link_delete(struct sw_link_list * list, struct link_list_element* link)
{
	if(list == NULL || list->head == NULL)
		return;
	struct link_list_element * tmp = list->head;
	struct link_list_element * curr = NULL;
	while(tmp)
	{
		if(tmp == link)
		{
			if(tmp == list->head)
			{
				list->head = tmp->next;
				list->link_num--;
				break;
			}
			else
			{
				curr->next = tmp->next;
				list->link_num--;
				link_list_element_free(tmp);
				break;
			}
		}
		curr = tmp;
		tmp = tmp->next;
	}
}

void path_link_delete(struct path_link_list * list, struct link_list_element* link)
{
	if(list == NULL || list->head == NULL)
		return;
	struct link_list_element * tmp = list->head;
	struct link_list_element * curr = NULL;
	while(tmp)
	{
		if(tmp == link)
		{
			if(tmp == list->head)
			{
				list->head = tmp->next;
				list->hop_num--;
				link_list_element_free(tmp);
				break;
			}
			else
			{
				curr->next = tmp->next;
				list->hop_num--;
				link_list_element_free(tmp);
				break;
			}
		}
		curr = tmp;
		tmp = tmp->next;
	}
}

void network_link_free(struct network_link * link)
{
	if(link)
		free(link);
}

void link_node_free(struct link_node * node)
{
	if(node)
		free(node);
}

void link_list_element_free(struct link_list_element* link)
{
	if(link)
		if(link->link)
			network_link_free(link->link);
		free(link);
}
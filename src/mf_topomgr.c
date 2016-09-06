#include "mf_topomgr.h"
#include "mf_switch.h"
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
		perror("topo mgr malloc failed\n");
		exit(0);
	}
	memset(LINK_NODE_CACHE_ARRAY, 0 , MF_TOPO_MGR.node_cache_array_size * sizeof(*LINK_NODE_CACHE_ARRAY));
	int i = 0;
	/*for(; i< MF_TOPO_MGR.node_cache_array_size; i++)
		push_to_array(LINK_NODE_CACHE_ARRAY + i, &(MF_TOPO_MGR.available_slot));
	MF_TOPO_MGR.used_slot = NULL;*/
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
	//MF_TOPO_MGR.next_available_index = 0;
}

/*static inline void push_to_array(struct link_node * value, struct link_node ** array)
{
	pthread_mutex_lock(&MF_TOPO_MGR.topomgr_mutex);
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
	pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
}

static inline struct link_node* pop_from_array(struct link_node * value, struct link_node ** array)
{
	pthread_mutex_lock(&MF_TOPO_MGR.topomgr_mutex);
	if(array == NULL || *array == NULL)
	{
		perror("Pop array is NULL");
		pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
		return NULL;
	}
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
				pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
				return tmp;
			}
			if(tmp->next == NULL)
			{
				tmp->prev->next = NULL;
				tmp->prev = NULL;
				pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
				return tmp;
			}
			if(tmp->prev && tmp->next)
			{
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
				pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
				return tmp;
			}
		}
		else
		{
			if(tmp->next)
				tmp = tmp->next;
			else
			{
				pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
				return NULL;
			}
		}
	}
	pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
	return NULL;
}*/

static void realloc_cache_array()
{
	pthread_mutex_lock(&MF_TOPO_MGR.topomgr_mutex);
	LINK_NODE_CACHE_ARRAY = (struct link_node *)realloc(LINK_NODE_CACHE_ARRAY, 2 * MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	if(LINK_NODE_CACHE_ARRAY == NULL)
	{
		perror("realloc cache array failed");
		exit(0);
	}
	memset(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size, 0, MF_TOPO_MGR.node_cache_array_size * sizeof(struct link_node));
	pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
	int i;
	/*for(i = MF_TOPO_MGR.node_cache_array_size; i< MF_TOPO_MGR.node_cache_array_size * 2; i++)
		push_to_array(LINK_NODE_CACHE_ARRAY + MF_TOPO_MGR.node_cache_array_size + i, &(MF_TOPO_MGR.available_slot));*/
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
	/*struct link_node * value = pop_from_array(MF_TOPO_MGR.available_slot, &(MF_TOPO_MGR.available_slot));*/
	return value;
	perror("No available value slot");
	return NULL;
}

struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port)
{
	if(port->node != NULL)
	{
		perror("Node already exists");
		return (port->node);
	}
	struct link_node * node = get_available_value_slot();
	if(node == NULL)
	{
		perror("Bad value slot");
		return NULL;
	}
	node->sw = sw;
	node->port = port;
	port->node= node;
	node->next = NULL;
	node->prev = NULL;
	node->is_occupied = 1;
	//push_to_array(node, &(MF_TOPO_MGR.used_slot));
	node->mem_manage_list.mark= 0;//Not necessary actually...But...Keep it as a good hobby..
	//lf_list_insert(l, &(MF_TOPO_MGR.used_list)); 
	MF_TOPO_MGR.total_node_number++;
	//printf("Node number is now: %ld\n", MF_TOPO_MGR.total_node_number);
	return node;
}

/*static uint32_t get_next_available_index()
{
	static uint8_t loop_restart;
	pthread_mutex_lock(&MF_TOPO_MGR.topomgr_mutex);
	uint32_t index = MF_TOPO_MGR.next_available_index;
	if(NETWORK_LINK_CACHE_ARRAY[index].is_occupied == 0)
	{
		MF_TOPO_MGR.total_network_link_number++;
		NETWORK_LINK_CACHE_ARRAY[index].is_occupied = 1;
	}
	else
	{
		if(MF_TOPO_MGR.total_network_link_number == MAX_NETWORK_LINK_NUM)  
		{
			perror("No available network link slot index");
			pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
			return MAX_NETWORK_LINK_NUM + 1;
		}
	}
	uint32_t tmp = index;
	if(tmp + 1 >= MAX_NETWORK_LINK_NUM)
		tmp = 0;
	while(NETWORK_LINK_CACHE_ARRAY[tmp].is_occupied == 1)
	{
		tmp++;
		if(tmp + 1 >= MAX_NETWORK_LINK_NUM && loop_restart == 0)
		{
			loop_restart = 1;
			tmp = 0;
		}
		else if(tmp+1 >= MAX_NETWORK_LINK_NUM && loop_restart == 1)
		{
			perror("No available network link slot index");
			loop_restart = 0;
			index = MAX_NETWORK_LINK_NUM + 1;
			break;
		}
	}
	MF_TOPO_MGR.next_available_index = tmp;
	pthread_mutex_unlock(&MF_TOPO_MGR.topomgr_mutex);
	return index;
}*/

struct network_link * network_link_create(struct link_node* src, struct link_node* dst)
{
	if(src->port->link == dst->port->link && src->port->link)
	{
		perror("Network Link already existes");
		printf("Network link num:%ld\n", MF_TOPO_MGR.total_network_link_number);
		return NULL;
	}
	//uint32_t index = get_next_available_index();
	struct lf_list * l = lf_list_pop(&(MF_TOPO_MGR.available_link_list));
	if(l == NULL)
	{
		printf("No available slot\n");
		return NULL;
	}
	struct network_link * link = container_of(l, struct network_link, mem_manage_list);
	lf_list_insert(l, &(MF_TOPO_MGR.used_link_list)); 
	MF_TOPO_MGR.total_network_link_number++;
	/*if(index == MAX_NETWORK_LINK_NUM + 1)
	  {
	  printf("No available slot\n");
	  return NULL;
	  }*/
	link->src = src;
	link->dst = dst;
	link->sw_next.next = NULL;
	link->sw_next.mark = 0;
	lf_list_insert(l, &(MF_TOPO_MGR.used_link_list));
	/*NETWORK_LINK_CACHE_ARRAY[index].src = src;
	  NETWORK_LINK_CACHE_ARRAY[index].dst = dst;
	  NETWORK_LINK_CACHE_ARRAY[index].sw_link_next = NULL;
	  src->port->link = & NETWORK_LINK_CACHE_ARRAY[index];
	  dst->port->link = & NETWORK_LINK_CACHE_ARRAY[index];
	  printf(" Network link num :%ld\n", MF_TOPO_MGR.total_network_link_number);
	  return (& NETWORK_LINK_CACHE_ARRAY[index]);*/
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
	if(link == NULL)
	{
		printf("return!\n");
		return 0;
	}
	if(sw_list->link_num == 0 && sw_list->head == NULL)
	{
		sw_list->head = link;
	}
	else
	{
		link->sw_link_next = sw_list->head;
		sw_list->head = link;
	}
	printf(" link inserted\n");
	sw_list->link_num++;
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
	struct network_link * tmp = src_sw->link_list.head;
	for(;i <= src_sw->link_list.link_num; i++)
	{
		if(tmp->dst->sw == dst_sw)


	}
}
*/

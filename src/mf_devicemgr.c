#include "mf_devicemgr.h"
#include "dbg.h"
#include "mf_switch.h"
#include "mf_utilities.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*=================
static Global variables
==================*/

struct mf_devicemgr MF_DEVICE_MGR;

static struct host_hash_value * HOST_HASH_MAP[HOST_HASH_MAP_SIZE];

static struct host_hash_value HOST_CACHE_ARRAY[MAX_HOST_NUM];

/*================
Functions
==================*/

static void push_to_array(struct host_hash_value * value, struct host_hash_value ** array);
static struct host_hash_value* hash_value_created(struct mf_switch *sw, uint32_t port_num, uint64_t mac_addr);
static uint8_t if_host_exist(struct host_hash_value * value, struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr);
static void used_to_available(struct host_hash_value * value);

void mf_devicemgr_create()
{
	MF_DEVICE_MGR.total_switch_number = 0;
	pthread_mutex_init(&(MF_DEVICE_MGR.devicemgr_mutex), NULL);
	MF_DEVICE_MGR.switches.next = NULL;
	MF_DEVICE_MGR.switches.mark = 0;
	int i = 0;
	MF_DEVICE_MGR.used_list.next = NULL;
	MF_DEVICE_MGR.used_list.mark = 0;
	MF_DEVICE_MGR.available_list.next = NULL;
	MF_DEVICE_MGR.available_list.mark= 0;
	for(i = 0; i < MAX_HOST_NUM; i++)
	{
		lf_list_insert(&(HOST_CACHE_ARRAY[i].mem_manage_list), &(MF_DEVICE_MGR.available_list));
  	}
	MF_DEVICE_MGR.available_slot_num = i;
	MF_DEVICE_MGR.used_slot_num = 0;
}

/*May return NULL
Programmer should take care of this*/
struct mf_switch * get_switch(uint32_t sock)
{
	return MF_DEVICE_MGR.mf_switch_map[sock];
}

void add_switch(struct mf_switch* sw)
{
	MF_DEVICE_MGR.mf_switch_map[sw->sockfd] = sw;
	lf_list_insert(&(sw->next_switch), &(MF_DEVICE_MGR.switches));
	MF_DEVICE_MGR.total_switch_number++;
}

void delete_switch_from_map(struct mf_switch * sw)
{
	pthread_mutex_lock(&sw->switch_mutex);
	MF_DEVICE_MGR.mf_switch_map[sw->sockfd] = NULL;
	lf_list_delete(&(sw->next_switch), &(MF_DEVICE_MGR.switches));
	MF_DEVICE_MGR.total_switch_number--;
	pthread_mutex_unlock(&sw->switch_mutex);
}
/*The upper limit of agrument loop_index
is decided by the the outer loop 

e.g.
int i = 0;
int intr_index = 0;
for(i = 0; i < curr_sw_number; i++)
{
	struct mf_switch * sw = get_next_switch(&intr_index);
	//Dosomething(sw);
}
This loop will return all the valid switches in the device manager storage.
And execute the Dosomething function to all the switches.
*/
struct mf_switch * get_next_switch(int* loop_index)
{
	if(*loop_index >= MAX_MF_SWITCH_NUM)
	{
		log_warn("Bad loop_index");
		return NULL;
	}
	for(; *loop_index < MAX_MF_SWITCH_NUM; (*loop_index)++)
	{ 
		if(MF_DEVICE_MGR.mf_switch_map[*loop_index] != NULL)
		{
			if(*loop_index < MAX_MF_SWITCH_NUM - 1)
			{
				(*loop_index)++; 
				return MF_DEVICE_MGR.mf_switch_map[(*loop_index) - 1];
			}
			else
			{
				return MF_DEVICE_MGR.mf_switch_map[*loop_index];
			}
		}
	}
	log_warn("No valid switch");
	return NULL;
}

inline static struct mf_switch * get_next_switch_from_list(struct mf_switch * sw)
{
	/*Keep the next_switch element at the top of struct mf_switch*/
	struct lf_list * l = sw->next_switch.next;
	return (struct mf_switch*)l;
}

struct mf_switch * get_switch_by_dpid(uint64_t dpid)
{
	int i,curr_index;
	curr_index = 0;
	for(i = 0; i < MF_DEVICE_MGR.total_switch_number; i++)
	{
		struct mf_switch* sw = get_next_switch(&curr_index);
		if(sw == NULL)
		{
			log_warn("No switch has this dpid");
			return NULL;
		}
		else if(sw->datapath_id == dpid)
		{
			return sw;
		}
	}
	log_warn("No switch has this dpid");
	return NULL;
}

struct mf_switch * get_switch_by_dpid_from_list(uint64_t dpid)
{
	struct mf_switch * tmp = get_next_switch_from_list((struct mf_switch *) &(MF_DEVICE_MGR.switches));
	while(tmp != NULL)	
	{
		if(tmp->datapath_id == dpid)
			return tmp;
		else
			tmp = get_next_switch_from_list(tmp);
  	}
	log_warn("No switch has this dpid:%ld", dpid);
	return NULL;
}

struct ofp11_port * get_switch_port_by_port_num(struct mf_switch* sw, ovs_be32 port_num)
{
	pthread_mutex_lock(&sw->switch_mutex);
	if(unlikely(sw == NULL))
	{
		log_warn("sw is NULL");
		pthread_mutex_unlock(&sw->switch_mutex);
		return NULL;
	}
	int i = 0;
	for(; i < sw->port_num; i++)	  
	{
		if(sw->ports[i].port_no == port_num) 
		{
			pthread_mutex_unlock(&sw->switch_mutex);
			return &(sw->ports[i]);
		}
	}
	pthread_mutex_unlock(&sw->switch_mutex);
	log_warn("No port has this port num");
	return NULL;
}
/*
static inline void push_to_array(struct host_hash_value * value, struct host_hash_value ** array)
{
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
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
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
}

static struct host_hash_value* pop_from_array(struct host_hash_value * value, struct host_hash_value ** array)
{
	if(array == NULL || *array == NULL)
	{
		log_warn("Array is NULL"); 
		return NULL;
	}
	struct host_hash_value * tmp = * array;
	while(tmp)
	{
		if(tmp == value)
		{
			if(tmp->prev == NULL)
			{
				* array = tmp->next;
				if(tmp->next != NULL)
					tmp->next->prev = NULL;
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
		{
			if(tmp->next)
				tmp = tmp->next;
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}

static struct host_hash_value * get_available_value_slot()
{
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
	if(MF_DEVICE_MGR.available_slot == NULL)
	{
		pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
		return NULL;
	}
	if(MF_DEVICE_MGR.available_slot->is_occupied == 0)
	{
		struct host_hash_value * value = pop_from_array(MF_DEVICE_MGR.available_slot, &(MF_DEVICE_MGR.available_slot));
		MF_DEVICE_MGR.available_slot_num--;
		MF_DEVICE_MGR.used_slot_num++;
		pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
		return value;
	}
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
	return NULL;
}*/

struct host_hash_value* host_hash_value_add(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr)
{
	uint64_t index = mac_addr_hash(mac_addr);
	struct host_hash_value * value = NULL;
	if(HOST_HASH_MAP[index] == NULL)
	{
		value = hash_value_created(sw, port_num, mac_addr); 
		HOST_HASH_MAP[index] = value;
		value->hash_map_slot_index = index;
		value->is_occupied = 1;
	}
	else
	{
		struct lf_list * tmp = &(HOST_HASH_MAP[index]->hash_list);
		while(tmp)
		{
			struct host_hash_value * value = container_of(tmp, struct host_hash_value, hash_list);
			if(if_host_exist(value, sw, port_num, mac_addr))
			{
				return NULL;
			}
			else
			{
				if(tmp->next== NULL)
				{
					value = hash_value_created(sw, port_num, mac_addr); 
					lf_list_insert(&(value->hash_list),&(HOST_HASH_MAP[index]->hash_list));
					value->hash_map_slot_index = index;
					value->is_occupied = 1;
					break;
				}	
				else
					tmp = tmp->next;
			}
		}
	}
	return value;
}

static inline uint8_t if_host_exist(struct host_hash_value * value, struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr)
{
	return !((value->mac_addr ^ mac_addr) | ((unsigned long)value->sw ^ (unsigned long)sw) | (value->port_num ^ port_num));
}

static struct host_hash_value* hash_value_created(struct mf_switch *sw, uint32_t port_num, uint64_t mac_addr)
{
	struct lf_list * l = lf_list_pop(&MF_DEVICE_MGR.available_list);
	struct host_hash_value * value = container_of(l, struct host_hash_value, mem_manage_list);
	lf_list_insert(l, &(MF_DEVICE_MGR.used_list));
	MF_DEVICE_MGR.available_slot_num--;
	MF_DEVICE_MGR.used_slot_num++;
	if(value != NULL)
	{
		value->mem_manage_list.next = NULL;
		value->mem_manage_list.mark = 0;
		value->hash_list.next = NULL;
		value->hash_list.mark = 0;
		value->sw = sw;
		lf_list_insert(&value->switch_list, &sw->hosts);
		value->port_num = port_num;
		value->mac_addr = mac_addr;
		return value;
	}
	log_warn("Value slot is NULL");
	return NULL;
}

inline uint32_t mac_addr_hash(uint64_t key)
{
	key = (~key) + (key << 21); // key = (key << 21) - key - 1; 
	key = key ^ (key >> 24); 
  	key = (key + (key << 3)) + (key << 8); // key * 265 
  	key = key ^ (key >> 14); 
  	key = (key + (key << 2)) + (key << 4); // key * 21 
  	key = key ^ (key >> 28); 
  	key = key + (key << 31);  
	return (key % HOST_HASH_MAP_SIZE); 
}

struct mf_switch * get_switch_by_host_mac(uint64_t mac_addr)
{
	uint32_t index = mac_addr_hash(mac_addr);
	if(HOST_HASH_MAP[index] == NULL)
		return NULL;
	else
	{
		struct lf_list * tmp = &(HOST_HASH_MAP[index]->hash_list);
		while(tmp)
		{
			struct host_hash_value * value = container_of(tmp, struct host_hash_value, hash_list);
			if(value->mac_addr == mac_addr)
				return value->sw;
			else
			{
				if(tmp->next)
					tmp = tmp->next;
				else
					return NULL;
			}
		}
		return NULL;
	}
}


//TODO: To test
void delete_host_hash_value(struct host_hash_value * value, struct host_hash_value * bucket_head)
{
	if(HOST_HASH_MAP[value->hash_map_slot_index] == NULL)	
	{
		log_warn("value is not exist");
		return;
	}
	struct host_hash_value * tmp = HOST_HASH_MAP[value->hash_map_slot_index];
	struct lf_list * link = &(tmp->hash_list);
	while(tmp)
	{
		if(tmp == value) 
		{
			break;
		}
		else
		{
			link = link->next;
			tmp = container_of(link, struct host_hash_value, hash_list);
		}
	}
	if(tmp == NULL)
	{
		log_warn("value is not exist");
		return;
	}
	lf_list_delete(link, &(bucket_head->hash_list));
	used_to_available(value);
}

static void used_to_available(struct host_hash_value * value)
{
	struct lf_list * tmp = lf_list_delete(&(value->hash_list), &(MF_DEVICE_MGR.used_list));
	lf_list_insert(tmp, &(MF_DEVICE_MGR.available_list));
}

void print_switch_link(struct mf_switch *sw)
{
	struct network_link* p = sw->link_list.head;
	while(p)
	{
		printf("sw_dpid: %ld\n ", p->src->sw->datapath_id);
		printf("src_sw_dpid:%ld, src_port_num: %d\n", p->src->sw->datapath_id, p->src->port->port_no);
		printf("dst_sw_dpid:%ld, dst_port_num: %d\n", p->dst->sw->datapath_id, p->dst->port->port_no);
		p = p->sw_link_next;
	}
}

void print_all_switches()
{
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
	int i = 0;
	int intr_index = 0;
	for(i = 0; i < MF_DEVICE_MGR.total_switch_number; i++)
	{
		struct mf_switch * sw = get_next_switch(&intr_index);
		if(sw == NULL)
			printf("sw is NULL\n");
		else
			print_switch_link(sw);
	} 
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
}

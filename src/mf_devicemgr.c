#include "mf_devicemgr.h"
#include "mf_switch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*=================
static Global variables
==================*/

static struct mf_devicemgr MF_DEVICE_MGR;

static struct host_hash_value * HOST_HASH_MAP[HOST_HASH_MAP_SIZE];

static struct host_hash_value HOST_CACHE_ARRAY[MAX_HOST_NUM];

/*================
Functions
==================*/
static void push_to_array(struct host_hash_value * value, struct host_hash_value ** array);

void mf_devicemgr_create()
{
	MF_DEVICE_MGR.total_switch_number = 0;
	pthread_mutex_init(&(MF_DEVICE_MGR.devicemgr_mutex), NULL);
	MF_DEVICE_MGR.available_slot = NULL;
	int i = 0;
	for(; i < MAX_HOST_NUM; i++)
	{
		push_to_array(&HOST_CACHE_ARRAY[i],&(MF_DEVICE_MGR.available_slot));
	}
	MF_DEVICE_MGR.used_slot = NULL;
}

struct mf_switch * get_switch(uint32_t sock)
{
	return MF_DEVICE_MGR.mf_switch_map[sock];
}

void add_switch(struct mf_switch* sw)
{
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
	MF_DEVICE_MGR.mf_switch_map[sw->sockfd] = sw;
	MF_DEVICE_MGR.total_switch_number++;
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
}

void delete_switch_from_map(struct mf_switch * sw)
{
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
	MF_DEVICE_MGR.mf_switch_map[sw->sockfd] = NULL;
	MF_DEVICE_MGR.total_switch_number--;
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
}

struct mf_switch * get_next_switch(uint32_t* loop_index)
{
	static uint32_t tmp;
	if(*loop_index == 0)
		tmp = 0;
	while(MF_DEVICE_MGR.mf_switch_map[tmp] == NULL)
		tmp++;
	return MF_DEVICE_MGR.mf_switch_map[tmp++];
}

struct mf_switch * get_switch_by_dpid(uint64_t dpid)
{
	uint32_t i;
	pthread_mutex_lock(&MF_DEVICE_MGR.devicemgr_mutex);
	for(i = 0; i < MF_DEVICE_MGR.total_switch_number; i++)
	{
		struct mf_switch* sw = get_next_switch(&i);
		if(sw->datapath_id == dpid)
		{
			pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
			return sw;
		}
	}
	pthread_mutex_unlock(&MF_DEVICE_MGR.devicemgr_mutex);
	return NULL;
}


static uint8_t is_struct_hash_value_identical(struct host_hash_value* a, struct host_hash_value* b)
{
	if(a->mac_addr == b->mac_addr \
		&& a->sw == b->sw \
		&& a->port_num == b->port_num)
		return 1;
	else
		return 0;
}


static void push_to_array(struct host_hash_value * value, struct host_hash_value ** array)
{
	if(*array == NULL)
	{
		*array = value;
		value->prev = NULL;
		value->next = NULL;
	}
	else
	{
		/*struct host_hash_value * tmp = *array;
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = value;
		value->prev = tmp;
		value->next = NULL;*/
		(*array)->prev = value;
		value->next = *array;
		value->prev = NULL;
		*array = value;
		
	}
}

static struct host_hash_value* pop_from_array(struct host_hash_value * value, struct host_hash_value ** array)
{
	if(array == NULL || *array == NULL)
		return NULL;
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
			if(tmp->next)
				tmp = tmp->next;
			else
				return NULL;
	}
	return NULL;
}

static struct host_hash_value * get_available_value_slot()
{
	if(MF_DEVICE_MGR.available_slot->is_occupied == 0)
	{
		struct host_hash_value * value = pop_from_array(MF_DEVICE_MGR.available_slot, &(MF_DEVICE_MGR.available_slot));
		return value;
	}
	return NULL;
}



struct host_hash_value* host_hash_value_add(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr)
{
	//struct host_hash_value * value = (struct host_hash_value * )malloc(sizeof(*value));
	struct host_hash_value * value = get_available_value_slot();
	if(value != NULL)
	{
		value->sw = sw;
		value->port_num = port_num;
		value->mac_addr = mac_addr;
		value->next = NULL;
		value->prev = NULL;
		value->hash_next = NULL;
		value->is_occupied = 1;
		push_to_array(value, &(MF_DEVICE_MGR.used_slot));
		host_add_to_hash_map(value);
		return value;
	}
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

void host_add_to_hash_map(struct host_hash_value* value)
{
	uint64_t index = mac_addr_hash(value->mac_addr);
	if(HOST_HASH_MAP[index] == NULL)
	{
		HOST_HASH_MAP[index] = value;
	}
	else
	{
		struct host_hash_value * tmp = HOST_HASH_MAP[index];
		while(tmp)
		{
			if(is_struct_hash_value_identical(tmp, value))
			{
				pop_from_array(value, &MF_DEVICE_MGR.used_slot);
				value->is_occupied = 0;
				push_to_array(value, &MF_DEVICE_MGR.available_slot);
				return;
			}
			/*
			TODO:
			Delete hash value structure which hash identical mac addr but different
			sw or port_num from the global hash map
			It happens when the same host connect to another port or switch
			*/
			if(tmp->hash_next)
				tmp = tmp->hash_next;
			else
			{
				tmp->hash_next = value;
				return;
			}
		}
	}
}

struct mf_switch * get_switch_by_host_mac(uint64_t mac_addr)
{
	uint32_t index = mac_addr_hash(mac_addr);
	if(HOST_HASH_MAP[index] == NULL)
		return NULL;
	else
	{
		struct host_hash_value * tmp = HOST_HASH_MAP[index];
		while(tmp)
		{
			if(tmp->mac_addr == mac_addr)
				return tmp->sw;
			else
			{
				if(tmp->hash_next)
					tmp = tmp->hash_next;
				else
					return NULL;
			}
		}
		return NULL;
	}
}

void host_hash_value_destory(struct host_hash_value* value)
{
	if(value)
		free(value);
}


void delete_host_hash_value(struct host_hash_value * value)
{

}
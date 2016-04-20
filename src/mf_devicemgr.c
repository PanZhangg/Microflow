#include "mf_devicemgr.h"
#include "mf_switch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*=================
Global variables
==================*/

struct mf_devicemgr MF_SWITCH_MAP;

static struct host_hash_value * HOST_HASH_MAP[HOST_HASH_MAP_SIZE];

void mf_devicemgr_create()
{
	MF_SWITCH_MAP.total_switch_number = 0;
	memset(&MF_SWITCH_MAP.mf_switch_map, 0, sizeof(MF_SWITCH_MAP.mf_switch_map));
	pthread_mutex_init(&(MF_SWITCH_MAP.devicemgr_mutex), NULL);
}

struct mf_switch * get_switch(uint32_t sock)
{
	return MF_SWITCH_MAP.mf_switch_map[sock];
}

void add_switch(struct mf_switch* sw)
{
	pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
	MF_SWITCH_MAP.mf_switch_map[sw->sockfd] = sw;
	MF_SWITCH_MAP.total_switch_number++;
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
}

void delete_switch_from_map(struct mf_switch * sw)
{
	pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
	MF_SWITCH_MAP.mf_switch_map[sw->sockfd] = NULL;
	MF_SWITCH_MAP.total_switch_number--;
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
}

struct mf_switch * get_next_switch(uint32_t* loop_index)
{
	static uint32_t tmp;
	if(*loop_index == 0)
		tmp = 0;
	while(MF_SWITCH_MAP.mf_switch_map[tmp] == NULL)
		tmp++;
	return MF_SWITCH_MAP.mf_switch_map[tmp++];
}

struct mf_switch * get_switch_by_dpid(uint64_t dpid)
{
	uint32_t i;
	pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
	for(i = 0; i < MF_SWITCH_MAP.total_switch_number; i++)
	{
		struct mf_switch* sw = get_next_switch(&i);
		if(sw->datapath_id == dpid)
		{
			pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
			return sw;
		}
	}
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
	return NULL;
}


static uint8_t is_hash_value_identical(struct host_hash_value* a, struct host_hash_value* b)
{
	if(a->mac_addr == b->mac_addr && a->sw == b->sw && a->port_num == b->port_num)
		return 1;
	else
		return 0;
}

struct host_hash_value* host_hash_value_create(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr)
{
	struct host_hash_value * value = (struct host_hash_value * )malloc(sizeof(*value));
	value->sw = sw;
	value->port_num = port_num;
	value->mac_addr = mac_addr;
	return value;
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
			if(is_hash_value_identical(tmp, value))
			{
				if((uint64_t)tmp == (uint64_t)value)
					return;
				else
					host_hash_value_destory(value);
				return;
			}
			/*
			TODO:
			Delet hash value structure which hash identical mac addr but different
			sw or port_num from the global hash map
			It happens when the same host connect to another port or switch
			*/
			if(tmp->next)
				tmp = tmp->next;
			else
			{
				tmp->next = value;
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
				if(tmp->next)
					tmp = tmp->next;
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

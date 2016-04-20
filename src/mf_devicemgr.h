#ifndef __MF_DEVICEMGR_H__
#define __MF_DEVICEMGR_H__

//#include "mf_switch.h"
#include "Openflow/types.h"
#include <pthread.h>

struct mf_switch;

#define MAX_MF_SWITCH_NUM 4096

extern struct mf_devicemgr MF_SWITCH_MAP;


struct mf_devicemgr
{
	uint32_t total_switch_number;
	struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];
	pthread_mutex_t devicemgr_mutex;
};
//uint32_t total_switch_number;
//extern struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];
void mf_devicemgr_create();

void add_switch(struct mf_switch * );

struct mf_switch * get_switch(uint32_t sock);

void delete_switch_from_map(struct mf_switch *);

struct mf_switch * get_next_switch(uint32_t* loop_index);//Usually used in a for-loop
/*
do_something for all the valid switches
Code template:
func()
{
unit32_t i;
pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
for(i = 0; i < MF_SWITCH_MAP.total_switch_number; i++)
{
	struct mf_switch* sw = get_switch_next_(&i);
	check & get(sw)
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
	do_something(sw);
}
}
*/

struct mf_switch * get_switch_by_dpid(uint64_t dpid);

#define HOST_HASH_MAP_SIZE 2048

struct host_hash_value
{
	struct mf_switch * sw;
	uint32_t port_num;
	uint64_t mac_addr;
	struct host_hash_value * next;
};

//extern struct host_hash_value HOST_HASH_MAP[HOST_HASH_MAP_SIZE];

struct host_hash_value* host_hash_value_create(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr);

inline uint32_t mac_addr_hash(uint64_t mac_addr);

void host_add_to_hash_map(struct host_hash_value* value);

struct mf_switch * get_switch_by_host_mac(uint64_t mac_addr);

void host_hash_value_destory(struct host_hash_value* value);

//void delete_hash_value(uint64_t mac_addr, )

#endif

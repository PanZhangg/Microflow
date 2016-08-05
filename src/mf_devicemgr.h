#ifndef __MF_DEVICEMGR_H__
#define __MF_DEVICEMGR_H__

#include <sched.h>
#include "Openflow/types.h"
#include <pthread.h>

struct mf_switch;

#define MAX_MF_SWITCH_NUM 4096

#define MAX_HOST_NUM 4096

struct mf_devicemgr
{
	uint32_t total_switch_number;
	struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];
	pthread_mutex_t devicemgr_mutex;
	/*
	always start with the available_slot
	to get a ptr pointed to static memory slot
	then store the ptr into used_slot
	*/
	struct host_hash_value * available_slot; //* available_slot head of the list
	struct host_hash_value * used_slot;
};

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
	unit32_t i,j;
	j = 0;
	pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
	for(i = 0; i < MF_SWITCH_MAP.total_switch_number; i++)
	{
		struct mf_switch* sw = get_switch_next_(&j);
		check(sw);
		do_something(sw);
	}
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
}
*/

struct mf_switch * get_switch_by_dpid(uint64_t dpid);
struct ofp11_port * get_switch_port_by_port_num(struct mf_switch* sw, ovs_be32 port_num);

#define HOST_HASH_MAP_SIZE 2048

/*
Hash map to store host information
Key: Host mac address
Value: struct host_hash_value
*/
struct host_hash_value
{
	struct mf_switch * sw;
	uint32_t port_num;
	uint64_t mac_addr;
	struct host_hash_value * next;
	struct host_hash_value * prev;
	struct host_hash_value * hash_next;
	uint8_t is_occupied;
};

struct host_hash_value* host_hash_value_add(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr);

inline uint32_t mac_addr_hash(uint64_t mac_addr);

void host_add_to_hash_map(struct host_hash_value* value);

struct mf_switch * get_switch_by_host_mac(uint64_t mac_addr);

void host_hash_value_destory(struct host_hash_value* value);

void delete_host_hash_value(struct host_hash_value * value);

#endif

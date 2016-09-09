#ifndef __MF_DEVICEMGR_H__
#define __MF_DEVICEMGR_H__

#include "Openflow/types.h"
#include <pthread.h>
#include "mf_lf_list.h"

struct mf_switch;

#define MAX_MF_SWITCH_NUM 4096

#define MAX_HOST_NUM 4096

#define HOST_MUTEX_SLOT_SIZE 8

#define HOST_HASH_MAP_SIZE 2048

struct mf_devicemgr
{
	uint32_t total_switch_number;
	struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];
	pthread_mutex_t devicemgr_mutex;
	struct lf_list available_list;
	struct lf_list used_list;
	uint32_t available_slot_num;
	uint32_t used_slot_num;
};

void mf_devicemgr_create();

void add_switch(struct mf_switch * );

struct mf_switch * get_switch(uint32_t sock);

void delete_switch_from_map(struct mf_switch *);

struct mf_switch * get_next_switch(int* loop_index);//Usually used within a for-loop
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


/*
Hash map to store host information
Key: Host mac address
Value: struct host_hash_value
*/
struct host_hash_value
{
	struct lf_list mem_manage_list;
	struct lf_list hash_list; 
	struct lf_list switch_list;
	struct mf_switch * sw;
	uint32_t port_num;
	uint64_t mac_addr;
	uint32_t host_array_slot_index;
	uint32_t hash_map_slot_index;
	uint8_t is_occupied;
};

struct host_hash_value* host_hash_value_add(struct mf_switch * sw, uint32_t port_num, uint64_t mac_addr);

inline uint32_t mac_addr_hash(uint64_t mac_addr);

struct mf_switch * get_switch_by_host_mac(uint64_t mac_addr);

void host_hash_value_destory(struct host_hash_value* value);

void delete_host_hash_value(struct host_hash_value * value, struct host_hash_value * bucket_head);

void print_switch_link(struct mf_switch *);

void print_all_switches();

#endif

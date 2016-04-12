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

struct mf_switch * get_all_switch_one_by_one(uint32_t* loop_index);//Usually used in a for-loop
/*
do_something for all the valid switches
Code template:
func()
{
unit32_t i;
pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
for(i = 0; i < MF_SWITCH_MAP.total_switch_number; i++)
{
	struct mf_switch* sw = get_all_switch_one_by_one(&i);
	check & get(sw)
	pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
	do_something(sw);
}
}
*/

struct mf_switch * get_switch_by_dpid(uint64_t dpid);

#endif

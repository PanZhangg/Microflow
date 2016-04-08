#include "mf_devicemgr.h"
#include "mf_switch.h"
#include <string.h>

struct mf_devicemgr MF_SWITCH_MAP;

void mf_devicemgr_create()
{
	MF_SWITCH_MAP.total_switch_number = 0;
	memset(&MF_SWITCH_MAP.mf_switch_map, 0, sizeof(MF_SWITCH_MAP.mf_switch_map));
	pthread_mutex_init(&(MF_SWITCH_MAP.devicemgr_mutex), NULL);
}

struct mf_switch * get_switch(uint32_t sock)
{
	//pthread_mutex_lock(&MF_SWITCH_MAP.devicemgr_mutex);
	return MF_SWITCH_MAP.mf_switch_map[sock];
	//pthread_mutex_unlock(&MF_SWITCH_MAP.devicemgr_mutex);
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

struct mf_switch * get_all_switch_one_by_one(uint32_t* loop_index)
{
	while(MF_SWITCH_MAP.mf_switch_map[*loop_index] == NULL)
	{
		*loop_index++;
	}
	return MF_SWITCH_MAP.mf_switch_map[*loop_index];
}
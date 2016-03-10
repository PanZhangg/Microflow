#ifndef __MF_DEVICEMGR_H__
#define __MF_DEVICEMGR_H__

#include "mf_switch.h"

#define MAX_MF_SWITCH_NUM 4096

uint32_t total_switch_number;
struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];

void add_switch(struct mf_switch* sw);

struct mf_switch * get_switch(uint32_t sock);

#endif

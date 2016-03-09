#ifndef __MF_DEVICEMGR_H__
#define __MF_DEVICEMGR_H__

#include "mf_socket.h"
#include "./Openflow/openflow-11.h"

#define MAX_MF_SWITCH_PORT_NUM 256
#define MAX_MF_SWITCH_NUM 1024



struct mf_switch
{
	struct mf_socket sw_socket;
	uint64_t datapath_id;
	uint32_t n_buffers;
	uint8_t n_tables;
	uint8_t auxiliary_id;
	uint32_t capabilities;
	struct ofp11_port[MAX_MF_SWITCH_PORT_NUM];
};

struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];

struct mf_switch * mf_switch_create(struct mf_socket sk);




#endif

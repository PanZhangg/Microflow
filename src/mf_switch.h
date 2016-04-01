#ifndef __MF_SWITCH_H__
#define __MF_SWITCH_H__
//#include "mf_rx_queue.h"
#include "./Openflow/openflow-1.1.h"

#define MAX_MF_SWITCH_PORT_NUM 256

struct mf_switch
{
	/*--socket--*/
	uint32_t sockfd;
	/*--openflow--*/
	uint64_t datapath_id;
	uint32_t n_buffers;
	uint8_t n_tables;
	uint8_t auxiliary_id;
	uint32_t capabilities;
	struct ofp11_port ports[MAX_MF_SWITCH_PORT_NUM];
	/*--msg control--*/
	uint8_t is_hello_sent;
	uint8_t is_feature_request_sent;
	/*--transcation ids--*/
	uint32_t feature_request_xid;
};

struct mf_switch * mf_switch_create(uint32_t sockfd);
void mf_switch_destory(struct mf_switch *);


#endif

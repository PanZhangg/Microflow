#ifndef __MF_SWITCH_H__
#define __MF_SWITCH_H__

#include "mf_socket.h"
#include "./Openflow/openflow-1.1.h"
#include "mf_topomgr.h"
#include <pthread.h>
#include "mf_lf_list.h"

#define MAX_MF_SWITCH_PORT_NUM 256

struct mf_switch
{
	/*network switch list*/
	/*Keep the next_switch element at the top of struct mf_switch*/
	struct lf_list next_switch;
	/*--socket--*/
	uint32_t sockfd;
	uint32_t epoll_recv_incomplete_length;
	/*--recv buffer--*/
	char rx_buffer[RX_BUFFER_SIZE];
	/*--openflow--*/
	uint64_t datapath_id;
	uint32_t n_buffers;
	uint8_t n_tables;
	uint8_t auxiliary_id;
	uint32_t capabilities;
	uint16_t port_num;
	struct ofp11_port ports[MAX_MF_SWITCH_PORT_NUM];
	/*--msg control--*/
	uint8_t is_hello_sent;
	uint8_t is_feature_request_sent;
	uint8_t is_port_desc_request_sent;
	/*--transcation ids--*/
	uint32_t feature_request_xid;
	/*--topo links --*/
	struct sw_link_list link_list;
	/*--thread mutex--*/	
	pthread_mutex_t switch_mutex;
	/*hosts list*/
	struct lf_list hosts;
};

struct mf_switch * mf_switch_create(uint32_t sockfd);
void mf_switch_destory(struct mf_switch *);

/*For debug purpose*/

void switch_print(struct mf_switch *);


#endif

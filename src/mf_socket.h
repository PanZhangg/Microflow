#ifndef __MF_SOCKET_H__
#define __MF_SOCKET_H__
  

#include "mf_rx_queue.h"
//#include "./Openflow/types.h"

#define DEFAULT_PORT 6633
#define EPOLL_EVENTS_NUM 256


struct mf_socket{
	uint32_t socket_fd;
	struct mf_rx_queue* rx_queue;
};

struct mf_socket mf_listen_socket_create();
struct mf_socket mf_socket_create(uint32_t fd);
void mf_socket_bind(struct mf_socket);
void handle_connection(struct mf_socket);
void destory_mf_socket(struct mf_socket);

#endif



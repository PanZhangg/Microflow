#ifndef __MF_SOCKET_H__
#define __MF_SOCKET_H__
  

#include "mf_rx_queue.h"
//#include "./Openflow/types.h"

#define DEFAULT_PORT 6633
#define EPOLL_EVENTS_NUM 256

uint32_t mf_listen_socket_create();
void mf_socket_bind(uint32_t);
void handle_connection(uint32_t);

#endif



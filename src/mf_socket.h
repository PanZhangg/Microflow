#ifndef __MF_SOCKET_H__
#define __MF_SOCKET_H__
  
#include "./Openflow/types.h"

#define DEFAULT_PORT 6633
#define EPOLL_EVENTS_NUM 1024

#define WORKER_THREADS_NUM 2

#define RX_BUFFER_SIZE 65535

struct mf_rx_queue;

extern struct mf_queue_node_mempool * MSG_RX_QUEUE[WORKER_THREADS_NUM];

uint32_t mf_listen_socket_create();
void mf_socket_bind(uint32_t);
void* handle_connection(void*);

#endif



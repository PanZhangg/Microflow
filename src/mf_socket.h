#ifndef __MF_SOCKET_H__
#define __MF_SOCKET_H__
  
#include "./Openflow/types.h"

#define DEFAULT_PORT 6633
#define EPOLL_EVENTS_NUM 1024
/*pthread_cond should be broadcast instead of signal if there are more than 1 worker threads
Code of push_queue_node_to_mempool in mf_mempool.c*/
#define WORKER_THREADS_NUM 1 /*--Do not change --*/


struct mf_rx_queue;

extern struct mf_queue_node_mempool * MSG_RX_QUEUE;

uint32_t mf_listen_socket_create();
void mf_socket_bind(uint32_t);
void handle_connection(uint32_t);

#endif



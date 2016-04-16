#ifndef MF_MEMPOOL_H__
#define MF_MEMPOOL_H__

#include "mf_rx_queue.h"
#include <pthread.h>

//#define MF_QUEUE_NODE_MEMPOOL_SIZE 262140
#define MF_QUEUE_NODE_MEMPOOL_SIZE 1024
struct mf_queue_node_mempool
{
	struct q_node node_pool[MF_QUEUE_NODE_MEMPOOL_SIZE];
	struct q_node * pop;
	struct q_node * push;
	struct q_node * head;
	struct q_node * tail;
	uint16_t valid_block_num;
	pthread_mutex_t pool_mutex;
	pthread_cond_t pool_cond;
};


struct mf_queue_node_mempool * mf_queue_node_mempool_create();
void push_queue_node_to_mempool(char* rx_buffer, uint16_t rx_length, struct mf_switch* sw, struct mf_queue_node_mempool* mp);
struct q_node * pop_queue_node_from_mempool(struct mf_queue_node_mempool* mp);
void free_memblock(struct q_node*, struct mf_queue_node_mempool* mp);

#endif

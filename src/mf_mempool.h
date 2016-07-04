#ifndef MF_MEMPOOL_H__
#define MF_MEMPOOL_H__

#include "mf_rx_queue.h"
#include <pthread.h>

//#define MF_QUEUE_NODE_MEMPOOL_SIZE 262140
//#define MF_QUEUE_NODE_MEMPOOL_SIZE 1024
#define MF_QUEUE_NODE_MEMPOOL_SIZE 8192

struct mf_queue_node_mempool
{
	struct q_node node_pool[MF_QUEUE_NODE_MEMPOOL_SIZE];
	struct q_node * pop;
	uint32_t p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15;//cache padding
	struct q_node * push;
	uint32_t p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29;//cache padding
	struct q_node * head;
	struct q_node * tail;
};


struct mf_queue_node_mempool * mf_queue_node_mempool_create();
void push_queue_node_to_mempool(char* rx_buffer, uint16_t rx_length, struct mf_switch* sw, struct mf_queue_node_mempool* mp);
struct q_node * pop_queue_node_from_mempool(struct mf_queue_node_mempool* mp);
void free_memblock(struct q_node*, struct mf_queue_node_mempool* mp);

#endif

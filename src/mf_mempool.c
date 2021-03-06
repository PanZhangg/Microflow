#include "mf_mempool.h"
#include "dbg.h"
#include "mf_logger.h"
#include "mf_utilities.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define barrier() __asm__ __volatile__("":::"memory")

static char queue_mem_pool[MF_QUEUE_NODE_MEMPOOL_SIZE][RX_PACKET_SIZE];

struct mf_queue_node_mempool * mf_queue_node_mempool_create()
{
	struct mf_queue_node_mempool * mp = (struct mf_queue_node_mempool*)malloc(sizeof(*mp));
	if(mp == NULL)
	{
		log_err("malloc failed");
		exit(0);
	}
	memset(mp, 0, sizeof(*mp));
	int i = 0;
	for( ; i< MF_QUEUE_NODE_MEMPOOL_SIZE; i++)
	{
  		mp->node_pool[i].rx_packet = &queue_mem_pool[i][0];
	}
	mp->head = &mp->node_pool[0];
	mp->tail = &mp->node_pool[MF_QUEUE_NODE_MEMPOOL_SIZE - 1];
	mp->pop = mp->head;
	mp->push = mp->head;
	return mp;
}

void push_queue_node_to_mempool(char* rx_buffer, uint16_t rx_length, struct mf_switch* sw, struct mf_queue_node_mempool* mp)
{
	if(unlikely((mp->pop > mp->head && mp->push == mp->pop - 1) || (mp->pop == mp->head && mp->push == mp->tail)))
	{
		log_warn("Queue is full, drop length:%d", rx_length);
		return;
	}
	memcpy(mp->push->rx_packet,rx_buffer, rx_length);
	mp->push->packet_length = rx_length;
	mp->push->sw = sw;
	if(unlikely(mp->push == mp->tail))
		mp->push = mp->head;
	else
		mp->push++;
}

struct q_node * pop_queue_node_from_mempool(struct mf_queue_node_mempool* mp)
{
	struct q_node * qn;
	if(unlikely(mp->pop == mp->push))
		return NULL;
	qn = mp->pop;
	if(unlikely(mp->pop == mp->tail))
		mp->pop = mp->head;
	else
		mp->pop++;
	return qn;
}

#include "mf_mempool.h"
#include "mf_logger.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct mf_queue_node_mempool * mf_queue_node_mempool_create()
{
	struct mf_queue_node_mempool * mp = (struct mf_queue_node_mempool*)malloc(sizeof(*mp));
	memset(mp, 0, sizeof(*mp));
	mp->head = &mp->node_pool[0];
	mp->tail = &mp->node_pool[MF_QUEUE_NODE_MEMPOOL_SIZE - 1];
	mp->pop = mp->head;
	mp->push = mp->head;
	mp->valid_block_num = 0;
	pthread_mutex_init(&mp->pool_mutex, NULL);
	pthread_cond_init(&mp->pool_cond, NULL);
	return mp;
}

void push_queue_node_to_mempool(char* rx_buffer, uint16_t rx_length, struct mf_switch* sw, struct mf_queue_node_mempool* mp)
{
	pthread_mutex_lock(&mp->pool_mutex);
	if(rx_length == 0)
	{
		pthread_mutex_unlock(&mp->pool_mutex);
		return;
	}
	if(mp->valid_block_num >= MF_QUEUE_NODE_MEMPOOL_SIZE)
	{
		printf("The MSG RX QUEUE is FULL, valid_block_num: %d\n", mp->valid_block_num);
		pthread_cond_broadcast(&mp->pool_cond);
		pthread_mutex_unlock(&mp->pool_mutex);
	}
	else
	{
		memcpy(mp->push->rx_packet,rx_buffer, rx_length);
		mp->push->packet_length = rx_length;
		mp->push->sw = sw;
		mp->push->is_occupied = 1;
		mp->valid_block_num++;
		printf("PUSH:valid_block_num:%d\n", mp->valid_block_num);
		pthread_cond_broadcast(&mp->pool_cond);
		if(mp->push == mp->tail)
			mp->push = mp->head;
		else
			mp->push++;
		printf("PUSH FINISHED:valid_block_num:%d\n", mp->valid_block_num);
		pthread_mutex_unlock(&mp->pool_mutex);
	}
}

struct q_node * pop_queue_node_from_mempool(struct mf_queue_node_mempool* mp)
{
	struct q_node * qn;
	if(mp->pop->is_occupied == 0 || mp->valid_block_num <= 0)
	{
		return NULL;
	}
	else
	{
		qn = mp->pop;
		mp->pop->is_occupied = 0;
		mp->valid_block_num--;
		printf("POP:valid_block_num:%d\n", mp->valid_block_num);
		if(mp->pop == mp->tail)
			mp->pop = mp->head;
		else
			mp->pop++;
		printf("POP:packet_length:%d", qn->packet_length);
		return qn;
	}
}
/*
void free_memblock(struct q_node* qn, struct mf_queue_node_mempool* mp)
{
	pthread_mutex_lock(&(mp->pool_mutex));
	qn->is_occupied = 0;
	pthread_mutex_unlock(&(mp->pool_mutex));
}
*/
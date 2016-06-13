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
	mp->seq_num = 0;
	mp->read_seq_num = 0;
	pthread_mutex_init(&mp->pool_mutex, NULL);
	pthread_cond_init(&mp->pool_cond, NULL);
	return mp;
}

void push_queue_node_to_mempool(char* rx_buffer, uint16_t rx_length, struct mf_switch* sw, struct mf_queue_node_mempool* mp)
{

	//if(mp->valid_block_num >= MF_QUEUE_NODE_MEMPOOL_SIZE)
	//{
	//	printf("The MSG RX QUEUE is FULL, valid_block_num: %d\n", mp->valid_block_num);
	//}
	//else
	//{
	//	pthread_mutex_lock(&mp->pool_mutex);	
		int index = mp->seq_num % MF_QUEUE_NODE_MEMPOOL_SIZE;
		memcpy(&(mp->node_pool[index].rx_packet), rx_buffer, rx_length);
		//memcpy(mp->push->rx_packet,rx_buffer, rx_length);
		mp->node_pool[index].packet_length = rx_length;
		mp->node_pool[index].sw = sw;
		mp->node_pool[index].is_occupied = 1;
		//mp->push->packet_length = rx_length;
		//mp->push->sw = sw;	
		//mp->push->is_occupied = 1;		
		//if(mp->push == mp->tail)
		//	mp->push = mp->head;
		//else
		//	mp->push++;
		//pthread_mutex_lock(&mp->pool_mutex);	
		__sync_fetch_and_add(&mp->seq_num, 1);
		//pthread_mutex_unlock(&mp->pool_mutex);
//		pthread_cond_broadcast(&mp->pool_cond);
	//}
}

struct q_node * pop_queue_node_from_mempool(struct mf_queue_node_mempool* mp)
{
	if(mp->read_seq_num - 1 > mp->seq_num)
	{
		__sync_fetch_and_sub(&mp->read_seq_num, 1);
		return NULL;
	}
	int index = (mp->read_seq_num - 1) % MF_QUEUE_NODE_MEMPOOL_SIZE;
	//if(mp->pop->is_occupied == 0 || mp->valid_block_num <= 0)
	//if(mp->pop->is_occupied == 0)
	if(mp->node_pool[index].is_occupied == 0)
	{
		return NULL;
	}
	else
	{
		struct q_node * qn;
		qn = &mp->node_pool[index];
		mp->node_pool[index].is_occupied = 0;
//		mp->valid_block_num--;
	//	pthread_mutex_lock(&mp->pool_mutex);
		//if(mp->pop == mp->tail)
		//	mp->pop = mp->head;
		//else
		//	mp->pop++;
	//	pthread_mutex_unlock(&mp->pool_mutex);
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

#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "mf_msg_parser.h"
#include "mf_msg_handler.h"
#include "mf_logger.h"
#include "mf_rx_queue.h"
#include "mf_switch.h"
#include "mf_socket.h"
#include "mf_mempool.h"
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"


void * worker_thread(void* arg)
{
	//cpu_set_t my_set;
	//CPU_ZERO(&my_set);
	//CPU_SET(1, &my_set);
	//pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &my_set);
	while(1)
	{
		pthread_mutex_lock(&MSG_RX_QUEUE->pool_mutex);
		while(MSG_RX_QUEUE->valid_block_num == 0)
		{
			pthread_cond_wait(&MSG_RX_QUEUE->pool_cond, &MSG_RX_QUEUE->pool_mutex);
		}
		while(MSG_RX_QUEUE->valid_block_num > 0)
		{
			struct q_node * qn = pop_queue_node_from_mempool(MSG_RX_QUEUE);
			parse_msg(qn);
		}
		pthread_mutex_unlock(&MSG_RX_QUEUE->pool_mutex);
	}
}

void parse_thread_start(uint8_t num)
{
	int i;
	for(i = 0; i < num; i++)
	{
		pthread_t thread_id;
		if((pthread_create(&thread_id, NULL, worker_thread, NULL)) < 0)
		{
			printf("thread create error\n");
		}
		pthread_detach(thread_id);
	}
}

static inline uint8_t parse_msg_type(struct q_node* qn)
{
	if(qn == NULL)
	{
		return 0;
	}
	uint8_t type;
	memcpy(&type, qn->rx_packet + 1, 1);
	return type;
}

static inline uint8_t parse_msg_version(struct q_node* qn)
{
	if(qn == NULL)
	{
		return 0;
	}
	uint8_t version;
	memcpy(&version, qn->rx_packet, 1);
	return version;
}

void parse_msg(struct q_node* qn)
{
	if(qn == NULL)
	{
		return;
	}
	else
	{
		uint8_t type = parse_msg_type(qn);
		uint8_t version = parse_msg_version(qn);
		msg_handler(type, version, qn);
	}

}


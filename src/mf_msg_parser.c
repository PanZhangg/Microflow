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

int queue_index[WORKER_THREADS_NUM];

void * worker_thread(void* arg)
{
	int index = *(int*)arg;
	while(1)
	{
		struct q_node * qn = pop_queue_node_from_mempool(MSG_RX_QUEUE[index]);
		if(qn == NULL)
			usleep(1);
		else
			parse_msg(qn);		
	}
}

void parse_thread_start(uint8_t num)
{
	int i;
	for(i = 0; i < num; i++)
	{
		pthread_t thread_id;
		queue_index[i] = i;
		if((pthread_create(&thread_id, NULL, worker_thread, (void*)&queue_index[i])) < 0)
		{
			perror("thread create error");
			exit(0);
		}
		pthread_detach(thread_id);
	}
}

static inline uint8_t parse_msg_type(struct q_node* qn)
{
	if(qn == NULL)
	{
		perror("qn is NULL when parsing msg type");
		return 0;
	}
	uint8_t type;
	//uint8_t type = (uint8_t)*(qn->rx_packet + 1);
	memcpy(&type, qn->rx_packet + 1, 1);
	return type;
}

static inline uint8_t parse_msg_version(struct q_node* qn)
{
	if(qn == NULL)
	{
		perror("qn is NULL when parsing version"); 
		return 0;
	}
	uint8_t version;
	//uint8_t version = (uint8_t)*qn->rx_packet;
	memcpy(&version, qn->rx_packet, 1);
	return version;
}

void parse_msg(struct q_node* qn)
{
	if(qn == NULL)
	{
		perror("qn is NULL when parsing msg");
		return;
	}
	else
	{
		uint8_t type = parse_msg_type(qn);
		//uint8_t version = parse_msg_version(qn);
		msg_handler(type, 4, qn);
	}

}


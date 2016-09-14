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
#include "mf_utilities.h"
#include "dbg.h"
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"

int queue_index[WORKER_THREADS_NUM];

void * worker_thread(void* arg)
{ 
	set_cpu_affinity();
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
			log_err("thread create error");
			exit(0);
		}
		pthread_detach(thread_id);
	}
}

static inline uint8_t parse_msg_type(struct q_node* qn)
{
	uint8_t type = *(qn->rx_packet + 1);
	return type;
}

static inline uint8_t parse_msg_version(struct q_node* qn)
{
	uint8_t version = *(qn->rx_packet);
	return version;
}

void parse_msg(struct q_node* qn)
{
		msg_handler(*(qn->rx_packet + 1), *(qn->rx_packet), qn);
}


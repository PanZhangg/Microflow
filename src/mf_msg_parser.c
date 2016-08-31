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
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"

int queue_index[WORKER_THREADS_NUM];

void * worker_thread(void* arg)
{ 
	static int cpu_id = 1;
	int cpunum = sysconf(_SC_NPROCESSORS_ONLN);
	if(cpunum >= 4)
	{
		int ccpu_id = 0;
		cpu_set_t my_set;
		if(__sync_bool_compare_and_swap(&cpu_id, 1, 2))
			ccpu_id = 2;
		else
			ccpu_id = 1;
		CPU_ZERO(&my_set);
		CPU_SET(ccpu_id, &my_set);
		//printf("Set CPU affinity: %d\n", ccpu_id);
		if(sched_setaffinity(0, sizeof(cpu_set_t), &my_set) == -1)
			perror("Set CPU affinity failed");
	}
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
	if(unlikely(qn == NULL))
	{
		perror("qn is NULL when parsing msg type");
		return 0;
	}
	//uint8_t type = (uint8_t)*(qn->rx_packet + 1);
	uint8_t type = 0;
	memcpy(&type, qn->rx_packet + 1, 1);
	return type;
}

static inline uint8_t parse_msg_version(struct q_node* qn)
{
	if(unlikely(qn == NULL))
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
	if(unlikely(qn == NULL))
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


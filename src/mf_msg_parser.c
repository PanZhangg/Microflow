#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "mf_msg_parser.h"
#include "mf_msg_handler.h"
#include "mf_logger.h"
#include "mf_rx_queue.h"
#include "mf_switch.h"
#include "mf_socket.h"
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"

void * worker_thread(void* arg)
{
	while(1)
	{
		struct q_node * qn = pop_q_node(MSG_RX_QUEUE);
		parse_msg(qn);
	}
}

void parse_thread_start(uint8_t num)
{
	int i;
	for(i = 1; i < num; i++)
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
	if(qn == NULL || qn->rx_packet == NULL)
	{
		return 0;
	}
	uint8_t type;
	memcpy(&type, qn->rx_packet + 1, 1);
	return type;
}

static inline uint8_t parse_msg_version(struct q_node* qn)
{
	if(qn == NULL || qn->rx_packet == NULL)
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


#include "mf_switch.h"
#include "mf_msg_parser.h"
#include "mf_logger.h"
#include "mf_devicemgr.h"
#include "mf_rx_queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

//extern struct mf_switch * mf_switch_map[MAX_MF_SWITCH_NUM];

/*void* handle_rx_msg(void* arg)
{
	struct mf_switch * sw = (struct mf_switch*)arg;
	while(1)
	{
		pthread_mutex_lock(&(sw->sw_mutex));
		if(sw->is_alive == 0)
		{
			pthread_mutex_unlock(&(sw->sw_mutex));
			destory_queue(sw->rxq);
			mf_switch_destory(sw);
			pthread_exit(0);
		}
		pthread_mutex_unlock(&(sw->sw_mutex));
		pthread_mutex_lock(&(sw->rxq->q_mutex));
		if(sw->rxq->queue_length == 0 || sw->rxq->head == NULL)
		{
			pthread_mutex_unlock(&(sw->rxq->q_mutex));
			continue;
		}
		else 
		{
			print_queue(sw->rxq);
			pthread_mutex_unlock(&(sw->rxq->q_mutex));
			struct q_node * qn = pop_q_node(sw->rxq);
			parse_msg(sw, qn);
		}
		
	}
}*/

struct mf_switch * mf_switch_create(uint32_t sockfd)
{
	struct mf_switch * sw = (struct mf_switch*)malloc(sizeof(struct mf_switch));
	sw->sockfd = sockfd;
	sw->datapath_id = 0;
	sw->n_buffers = 0;
	sw->n_tables = 0;
	sw->auxiliary_id = 0;
	sw->capabilities = 0;
	memset(&(sw->ports), 0, sizeof(sw->ports));
	sw->is_hello_sent = 0;
	sw->is_feature_request_sent = 0;
	sw->is_port_desc_request_sent = 0;
	sw->feature_request_xid = 0;
	memset(&(sw->links), 0, sizeof(sw->links));
	add_switch(sw);
	return sw;
}

void mf_switch_destory(struct mf_switch * sw)
{
	if(sw == NULL)
	{
		printf("error: switch to destory is NULL\n");
		exit(0);
	}
	if(close(sw->sockfd) < 0)
	{
		printf("socket close error\n");
	}
	delete_switch_from_map(sw);
	free(sw);
}


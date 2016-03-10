#include "mf_switch.h"
#include "mf_msg_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


void* handle_rx_msg(void* arg)
{
	struct mf_switch * sw = (struct mf_switch*)arg;
	while(1)
	{
		while(sw->rxq->queue_length == 0);
		print_queue(sw->rxq);
		struct q_node * qn = pop_q_node(sw->rxq);
		parse_msg(sw, qn);
		//qn-> socket_fd= 0;
	}
}

struct mf_switch * mf_switch_create(uint32_t sockfd)
{
	struct mf_switch * sw = (struct mf_switch*)malloc(sizeof(*sw));
	sw->sockfd = sockfd;
	sw->rxq = mf_rx_queue_init();
	sw->pid = pthread_create(&(sw->pid), NULL, handle_rx_msg, (void*)sw);
	if(sw->pid < 0)
	{
		//fprintf(stderr, "error: pthread_create, pid: %d\n", sw->pid);
		printf("error: pthread create failed\n");
		exit(0);
	}
	sw->datapath_id = 0;
	sw->n_buffers = 0;
	sw->n_tables = 0;
	sw->auxiliary_id = 0;
	sw->capabilities = 0;
	memset(&(sw->ports), 0, sizeof(sw->ports));
	sw->is_hello_sent = 0;

	return sw;
}

void mf_switch_destory(struct mf_switch * sw)
{
	if(sw == NULL)
	{
		//fprintf(stderr, "error: switch to destory is NULL: %d\n", sw->pid);
		printf("error: switch to destory is NULL\n");
		exit(0);
      	//return EXIT_FAILURE;
	}
	if(sw->rxq)
		destory_queue(sw->rxq);
	//pthread_exit(sw->pid);
	pthread_cancel(sw->pid);
	pthread_join(sw->pid, NULL);
	close(sw->sockfd);
	free(sw);
}


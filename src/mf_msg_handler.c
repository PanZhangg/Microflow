#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "mf_msg_handler.h"
#include "mf_ofmsg_constructor.h"


//void echo_msg_handler(q_node*);
//void packet_in_msg_handler(q_node*);
//void flow_remove_msg_handler(q_node*);

struct mf_msg_handler* msg_handler_init(struct mf_rx_queue* mrq, msg_handler_func mhf)
{
	struct mf_msg_handler* mmh = (struct mf_msg_handler*)malloc(sizeof(*mmh));
	mmh->msg_queue = mrq;
	mmh->mhf = mhf;
	return mmh;
}
void msg_handler_destory(struct mf_msg_handler* mmh)
{
	if(mmh && mmh->msg_queue)
	{
		destory_queue(mmh->msg_queue);
		free(mmh);
	}
	else if(mmh)
		free(mmh);
}

void hello_msg_handler(struct q_node* qn)
{
	//TODO: mutex needs to be fixed
	//probably in the while loop of hello_msg_handler_thread_func
	//lock the queue before reading the value
	//Goal: make qn==NULL impossible
	//printf("\nHello msg handling\n");
	uint32_t xid;
	memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	send(qn->socket_fd, &oh, sizeof(oh), MSG_DONTWAIT);
	printf("\nHello msg handling");
	destory_q_node(qn);
	//struct ofp_header* header= (struct ofp_header*)(qn->rx_packet)
}


void* hello_msg_handler_thread_func(void* argv)
{
	struct mf_msg_handler* mmh = (struct mf_msg_handler*)argv;
	while(1)
	{
		//while(mmh->msg_queue->head)
		while(mmh->msg_queue->queue_length)
		{
			struct q_node* qn = pop_q_node(mmh->msg_queue);
			mmh->mhf(qn);
		}
	}
}

void hello_msg_handler_thread_start()
{
	struct mf_msg_handler* hello_msg_mmh = msg_handler_init(Hello_rx_message_queue, &hello_msg_handler);
	pthread_t hello_msg_handler_thread;
	int rc;
	if ((rc = pthread_create(&hello_msg_handler_thread, NULL, hello_msg_handler_thread_func, (void*)hello_msg_mmh)))
	{
      //fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      //return EXIT_FAILURE;
    }
    pthread_detach(hello_msg_handler_thread);
}


void Msg_handler_thread_start()
{
	hello_msg_handler_thread_start();
}
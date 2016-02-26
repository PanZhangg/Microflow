#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "mf_msg_parser.h"
#include "mf_socket_array.h"
#include "mf_msg_handler.h"
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"



static void msg_queue_init()
{
	Hello_rx_message_queue = mf_rx_queue_init();
	Echo_rx_message_queue = mf_rx_queue_init();
	Packet_in_rx_message_queue = mf_rx_queue_init();
}

static inline uint8_t parser_msg_type(struct q_node* qn)
{
	//char* content = (char*)(qn->rx_packet);
	//printf("\nparser_msg_type:Packet content:%s\n", content);
	uint8_t type;
	//printf("\nruning here\n");
	memcpy(&type, qn->rx_packet + 1, 1);
	//printf("\ntype:%d\n", type);
	return type;
}

static struct q_node* get_q_node(struct mf_socket_array_node* san)
{
	if((san->s.rx_queue)->queue_length == 0)
		return NULL;
	else
	{
		struct q_node* tmp = pop_q_node(san->s.rx_queue);
		//char* content = (char*)(tmp->rx_packet);
		//printf("\nPacket content:%s\n", content);
		//return pop_q_node(san->s.rx_queue);
		return tmp;
	}
}

/*static struct mf_socket_array_node* get_socket_array_node()
{

}
*/
static void push_msg_queue(struct q_node* qn)
{
	//char* content = (char*)(qn->rx_packet);
	//printf("\npush_msg_:Packet content:%s\n", content);
	uint8_t type = parser_msg_type(qn);
	//printf("\nstep out of parser_msg_type_func");
	printf("\ntype:%d", type);
	//type++;
	if(!push_q_node(qn, Hello_rx_message_queue))
		printf("push queue failed");
	//print_queue(Hello_rx_message_queue);
	//printf("Got msg");
	/*switch(type)
	{
		case HELLO :
			push_q_node(qn, Hello_rx_message_queue);
			break;
	}
	*/
}

void* parser_work(void* arg)
{
	struct mf_socket_array_node * tmp = NULL;
	//tmp = mf_socket_array->head;
	while(1)
	{
		if(mf_socket_array->head != NULL)
			tmp = mf_socket_array->head;
		while(tmp)
		{
				struct q_node* qn = get_q_node(tmp);
				if(qn == NULL)
				{
					goto goto_next_node;
					/*if(tmp->next_node == NULL)
						tmp = mf_socket_array->head;
					else
						tmp = tmp->next_node;*/
				}
				else
				{
					push_msg_queue(qn);
goto_next_node:
					if(tmp->next_node == NULL)
						tmp = mf_socket_array->head;
					else
						tmp = tmp->next_node;
				}
		}	
	}
}


void parser_thread_start()
{
	pthread_t parser_thread;
	int rc;
	msg_queue_init();
	if ((rc = pthread_create(&parser_thread, NULL, parser_work, NULL)))
	{
      //fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      //return EXIT_FAILURE;
    }
    pthread_detach(parser_thread);
	Msg_handler_thread_start();
}

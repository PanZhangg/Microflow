#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "mf_msg_parser.h"
#include "mf_socket_array.h"
#include "mf_msg_handler.h"
#include "./Openflow/openflow.h"
#include "./Openflow/openflow-common.h"

extern pthread_mutex_t socket_array_mutex;

static void msg_queue_init()
{
	Hello_rx_message_queue = mf_rx_queue_init();
	Echo_rx_message_queue = mf_rx_queue_init();
	Packet_in_rx_message_queue = mf_rx_queue_init();
}

static inline uint8_t parser_msg_type(struct q_node* qn)
{
	uint8_t type;
	memcpy(&type, qn->rx_packet + 1, 1);
	return type;
}

static inline uint8_t parser_msg_version(struct q_node* qn)
{
	uint8_t version;
	memcpy(&version, qn->rx_packet, 1);
	return version;
}

static struct q_node* get_q_node(struct mf_socket_array_node* san)
{
	if((san->s.rx_queue)->queue_length == 0)
		return NULL;
	else
	{
		struct q_node* tmp = pop_q_node(san->s.rx_queue);
		return tmp;
	}
}

static void push_msg_queue(struct q_node* qn)
{
	uint8_t version = parser_msg_version(qn);
	uint8_t type = parser_msg_type(qn);
	//printf("\ntype:%d", type);
	if(version ==4 && type == 0)
	{
		if(!push_q_node(qn, Hello_rx_message_queue))
			printf("push queue failed");
	}
}

void* parser_work(void* arg)
{
	struct mf_socket_array_node * tmp = NULL;

	while(1)
	{
		//pthread_mutex_lock(&socket_array_mutex);
parser_start:	
		//pthread_mutex_lock(&socket_array_mutex);
		if(mf_socket_array->array_length != 0)
		{
			tmp = mf_socket_array->head;
		//	pthread_mutex_unlock(&socket_array_mutex);
		}
		else
		{
			tmp = NULL;
		//	pthread_mutex_unlock(&socket_array_mutex);
		}
			//break;
			//continue;
		//pthread_mutex_unlock(&socket_array_mutex);
		while(tmp)
		{
				//pthread_mutex_lock(&socket_array_mutex);
				struct q_node* qn = get_q_node(tmp);
				//pthread_mutex_unlock(&socket_array_mutex);
				if(qn == NULL)
				{
					goto goto_next_node;
				}
				else
				{
					push_msg_queue(qn);
goto_next_node:
					//pthread_mutex_lock(&socket_array_mutex);
					if(tmp->next_node == NULL)
					{
						if(mf_socket_array->array_length == 0)
						{
							goto parser_start;
							//printf("array head == NULL\n");
							//tmp = NULL;
							//tmp = mf_socket_array->head;
							//exit(0);
						}
						else
						{
							tmp = mf_socket_array->head;
						}
						//pthread_mutex_unlock(&socket_array_mutex);
					}
					else
						tmp = tmp->next_node;
				}
				//pthread_mutex_unlock(&socket_array_mutex);
		}
		//pthread_mutex_unlock(&socket_array_mutex);	
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

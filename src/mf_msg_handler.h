#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"

typedef void (*msg_handler_func)(struct q_node*);

extern struct mf_rx_queue* Hello_rx_message_queue;

struct mf_msg_handler{
	struct mf_rx_queue* msg_queue;
	msg_handler_func mhf;
};

struct mf_msg_handler* msg_handler_init(struct mf_rx_queue*, msg_handler_func);
void msg_handler_destory(struct mf_msg_handler* );
//void* hello_msg_handler(void* );
//void echo_msg_handler(q_node*);
//void packet_in_msg_handler(q_node*);
//void flow_remove_msg_handler(q_node*);


void Msg_handler_thread_start();




#endif

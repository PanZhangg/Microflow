#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"

typedef void (*msg_handler_func)(struct q_node*);

void hello_msg_handler(q_node*);
void echo_msg_handler(q_node*);
void packet_in_msg_handler(q_node*);
void flow_remove_msg_handler(q_node*);


void msg_handler_thread_start(msg_handler_func mhf);



#endif

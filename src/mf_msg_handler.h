#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"
#include "mf_switch.h"

extern struct mf_queue_node_mempool * MSG_RX_QUEUE;
typedef void (*msg_handler_func)(struct q_node*);

void msg_handler(uint8_t type, uint8_t version, struct q_node* qn);
void hello_msg_handler(struct q_node*);
void echo_request_handler(struct q_node* qn);

#endif

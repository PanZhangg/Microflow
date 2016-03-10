#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"
#include "mf_switch.h"

typedef void (*msg_handler_func)(struct q_node*);

void msg_handler(struct mf_switch*, uint8_t type, uint8_t version, struct q_node* qn);
void hello_msg_handler(struct mf_switch*, struct q_node*);

#endif

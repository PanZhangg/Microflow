#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"
#include "mf_switch.h"

extern struct mf_queue_node_mempool * MSG_RX_QUEUE;
typedef void (*msg_handler_func)(struct q_node*);

void msg_handler(uint8_t type, uint8_t version, struct q_node* qn);
void hello_msg_handler(struct q_node*);
void echo_request_handler(struct q_node* qn);
void send_packet_out(uint32_t buffer_id, uint32_t in_port, void* data, uint32_t data_length, void* action, uint16_t action_length);

#endif

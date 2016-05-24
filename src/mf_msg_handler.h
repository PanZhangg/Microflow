#ifndef __MF_MSG_HANDLER__
#define __MF_MSG_HANDLER__

#include "mf_rx_queue.h"
#include "mf_switch.h"


enum MSG_HANDLER_TYPE{
	HELLO_MSG_HANDLER_FUNC,
	ECHO_REQUEST_HANDLER_FUNC,
	FEATURE_REPLY_HANDLER_FUNC,
	PACKET_IN_MSG_HANDLER_FUNC
};


typedef void (*msg_handler_func)(struct q_node*);


/*Msg handler structure used to register defualt and customlized handler functions
For each type of msg, a linked list of handler func pointors could be created and the 
certain type of msg will go through the whole list in order to satisfy certain requirements
*/
struct single_msg_handler
{
	msg_handler_func handler_func;
	struct single_msg_handler * next;
};

struct msg_handlers
{
	struct single_msg_handler * hello_msg_handler_list_head;
	struct single_msg_handler * echo_request_handler_list_head;
	struct single_msg_handler * feature_reply_handler_list_head;
	struct single_msg_handler * packet_in_msg_handler_list_head;
};

void msg_handler(uint8_t type, uint8_t version, struct q_node* qn);
void hello_msg_handler(struct q_node*);
void echo_request_handler(struct q_node* qn);
void send_packet_out(struct q_node* qn, uint32_t xid, uint32_t buffer_id, void* data, uint32_t data_length);
void send_multipart_port_request(struct q_node* qn);
void feature_reply_handler(struct q_node* qn);
void packet_in_msg_handler(struct q_node*);
void multipart_reply_handler(struct q_node* qn);

void arp_msg_handler(struct q_node* qn, uint32_t xid, char* buffer, uint16_t total_len);
void lldp_msg_handler(struct q_node* qn, uint32_t xid, char* buffer, uint16_t total_len);
/*functions needed to be implemented*/

void msg_handlers_init(); //register default msg hander func to MSG_HANDLERS
struct single_msg_handler * single_msg_handler_create(msg_handler_func);
void msg_handler_func_register(enum MSG_HANDLER_TYPE, msg_handler_func);
void msg_handler_func_unregister(enum MSG_HANDLER_TYPE, msg_handler_func);
#endif

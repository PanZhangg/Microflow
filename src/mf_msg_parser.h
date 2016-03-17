#ifndef __MF_MSG_PARSER__
#define __MF_MSG_PARSER__


#include "Openflow/types.h"
//#include "mf_rx_queue.h"
//#include "mf_switch.h"
struct q_node;
struct mf_switch;

void parse_thread_start(uint8_t num);

void parse_msg(struct q_node* );


#endif

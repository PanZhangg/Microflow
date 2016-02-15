#ifndef __MF_MSG_PARSER__
#define __MF_MSG_PARSER__


#include "mf_rx_queue.h"


extern struct mf_socket_array * mf_socket_array;

struct mf_rx_queue* Hello_rx_message_queue;
struct mf_rx_queue* Echo_rx_message_queue;


void parser_thread_start();

#endif

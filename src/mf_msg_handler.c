#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "mf_msg_handler.h"
#include "mf_ofmsg_constructor.h"
#include "mf_mempool.h"
#include "mf_logger.h"

void msg_handler(uint8_t type, uint8_t version, struct q_node* qn)
{
	printf("msg received\n");

	if(version == 4)
	{
		switch(type)
		{
			case 0: hello_msg_handler(qn); break;
			case 2: echo_request_handler(qn); break;
			default: printf("Invalid msg type\n"); break;
		}
	}
	else
		printf("Msg is not Openflow Version 1.3\n");

	free_memblock(qn, MSG_RX_QUEUE);
}

void hello_msg_handler(struct q_node* qn)
{
	//mf_write_socket_log("Hello Message received", qn->sw->sockfd);
	uint32_t xid;
	memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	if(qn->sw->is_hello_sent == 0)
	{
		send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
		//mf_write_socket_log("Hello Message sent", qn->sw->sockfd);
		qn->sw->is_hello_sent = 1;
	}
	else
	{
		
	}
	if(qn->sw->is_feature_request_sent == 0)
	{

	}
	printf("Hello msg handling\n");
}

void echo_request_handler(struct q_node* qn)
{
	//mf_write_socket_log("Echo Message received", qn->sw->sockfd);
	uint32_t xid;
	memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_echo_reply_msg_constructor(xid);
	send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
	//mf_write_socket_log("Echo Message sent", qn->sw->sockfd);
	printf("echo reply msg send\n");
}



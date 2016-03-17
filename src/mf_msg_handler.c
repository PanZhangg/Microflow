#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "mf_msg_handler.h"
#include "mf_ofmsg_constructor.h"

void msg_handler(uint8_t type, uint8_t version, struct q_node* qn)
{
	//printf("msg received\n");
	if(version == 4 && type == 0)
	{
		hello_msg_handler(qn);
	}
	destory_q_node(qn);
}

void hello_msg_handler(struct q_node* qn)
{

	uint32_t xid;
	memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	if(qn->sw->is_hello_sent == 0)
	{
		send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
		qn->sw->is_hello_sent = 1;
	}
	else
	{
		
	}
	printf("Hello msg handling\n");
}



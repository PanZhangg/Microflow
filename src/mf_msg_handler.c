#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "mf_msg_handler.h"
#include "mf_ofmsg_constructor.h"

void msg_handler(struct mf_switch* sw, uint8_t type, uint8_t version, struct q_node* qn)
{
	if(version == 4 && type == 0)
	{
		hello_msg_handler(sw, qn);
	}
}

void hello_msg_handler(struct mf_switch* sw, struct q_node* qn)
{
	uint32_t xid;
	memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	if(sw->is_hello_sent == 0)
	{
		send(qn->socket_fd, &oh, sizeof(oh), MSG_DONTWAIT);
		sw->is_hello_sent = 1;
	}
	else
	{
		
	}
	printf("Hello msg handling\n");
	destory_q_node(qn);
}



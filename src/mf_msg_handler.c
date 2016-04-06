#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "mf_msg_handler.h"
#include "mf_ofmsg_constructor.h"
#include "mf_mempool.h"
#include "mf_logger.h"
#include "mf_timer.h"
#include "mf_utilities.h"


/*=====================================
Functions for msg handlers
======================================*/


void hello_msg_stopwatch_callback(void* arg) //for timer function test
{
	if(arg == NULL)
	{
		printf("stopwatch callback arg is null\n");
		return;
	}else{
		struct q_node * qn = (struct q_node *)arg;
		printf("Print this msg every 1 sec\n");
	}
	
}

static void send_switch_features_request(struct q_node* qn)
{
	uint32_t xid = generate_random();
	qn->sw->feature_request_xid = xid;
	struct ofp_header oh = of13_switch_feature_msg_constructor(xid);
	send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
	mf_write_socket_log("Feature_request Message sent", qn->sw->sockfd);
}


void send_packet_out(struct q_node* qn, uint32_t buffer_id, void* data, uint32_t data_length)
{
	char packet_out_buffer[1024];
	uint32_t xid = generate_random();
	struct ofp_header oh = ofp13_msg_header_constructor(xid, 13, data_length + 16 + 8);
	struct ofp11_packet_out pkt = of13_packet_out_msg_constructor(buffer_id, 16);
	struct ofp_action_output oao = ofp13_action_output_constructor(1);
	memcpy(packet_out_buffer, &oh, sizeof(oh));
	memcpy(packet_out_buffer+sizeof(oh), &pkt, sizeof(pkt));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt), &oao, sizeof(oao));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt)+sizeof(oao), data, data_length);
	send(qn->sw->sockfd, &packet_out_buffer, data_length+16+8+sizeof(pkt), MSG_DONTWAIT);
}

/*=====================================
Msg handler functions
======================================*/


void msg_handler(uint8_t type, uint8_t version, struct q_node* qn)
{
	printf("msg received\n");
	if(version == 4)
	{
		switch(type)
		{
			case 0: hello_msg_handler(qn); break;
			case 2: echo_request_handler(qn); break;
			case 6: feature_reply_handler(qn); break;
			default: printf("Invalid msg type\n"); break;
		}
	}
	else
		printf("Msg is not Openflow Version 1.3\n");
	free_memblock(qn, MSG_RX_QUEUE);
}

void hello_msg_handler(struct q_node* qn)
{
	static uint8_t is_timer_added;
	if(is_timer_added == 0)
	{
		//struct stopwatch * spw = stopwatch_create(1.0, &hello_msg_stopwatch_callback, PERMANENT, (void*)qn);
		is_timer_added = 1;
	}
	mf_write_socket_log("Hello Message received", qn->sw->sockfd);
	uint32_t xid;
	inverse_memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	if(qn->sw->is_hello_sent == 0)
	{
		send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
		mf_write_socket_log("Hello Message sent", qn->sw->sockfd);
		qn->sw->is_hello_sent = 1;
	}
	else
	{
		
	}
	if(qn->sw->is_feature_request_sent == 0)
	{
		send_switch_features_request(qn);
		qn->sw->is_feature_request_sent = 1;
	}
	printf("Hello msg handling\n");
}

void echo_request_handler(struct q_node* qn)
{
	mf_write_socket_log("Echo Message received", qn->sw->sockfd);
	uint32_t xid;
	inverse_memcpy(&xid, qn->rx_packet + 4, 4);
	struct ofp_header oh = of13_echo_reply_msg_constructor(xid);
	send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
	mf_write_socket_log("Echo Message sent", qn->sw->sockfd);
	printf("echo reply msg send\n");
}


void feature_reply_handler(struct q_node* qn)
{
	static int count;
	mf_write_socket_log("feature_reply Message received", qn->sw->sockfd);
	printf("feature_reply message received\n");
	//memcpy(&qn->sw->datapath_id, qn->rx_packet + 8, 8);
	inverse_memcpy(&qn->sw->datapath_id, qn->rx_packet + 8, 8);
	//memcpy(&qn->sw->n_buffers, qn->rx_packet + 16, 4);
	inverse_memcpy(&qn->sw->n_buffers, qn->rx_packet + 16, 4);
	memcpy(&qn->sw->n_tables, qn->rx_packet + 20, 1);
	memcpy(&qn->sw->auxiliary_id, qn->rx_packet + 21, 1);
	//memcpy(&qn->sw->capabilities, qn->rx_packet + 24, 4);
	inverse_memcpy(&qn->sw->capabilities, qn->rx_packet + 24, 4);
	printf("dpid:%lld\n",qn->sw->datapath_id);
	printf("buffers:%d\n",qn->sw->n_buffers);
	printf("tables:%d\n",qn->sw->n_tables);
	count++;
	printf("count:%d\n", count);
}


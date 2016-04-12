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
Global variables
======================================*/

struct msg_handlers * MSG_HANDLERS;


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
		//struct q_node * qn = (struct q_node *)arg;
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

void send_multipart_port_desc_request(struct q_node* qn)
{
	struct ofp_multipart_request omr = of13_multiaprt_request_constructor(13, 0);
	send(qn->sw->sockfd, &omr, sizeof(omr), MSG_DONTWAIT);
}


void send_packet_out(struct q_node* qn, uint32_t buffer_id, void* data, uint32_t data_length)
{
	char packet_out_buffer[1024];
	//char* buffer_ptr = packet_out_buffer;
	uint32_t xid = generate_random();
	struct ofp_header oh = ofp13_msg_header_constructor(xid, 13, data_length + 16 + 8);
	struct ofp11_packet_out pkt = of13_packet_out_msg_constructor(buffer_id, 16);
	struct ofp_action_output oao = ofp13_action_output_constructor(1);
	/*TODO:
	 *quick code for testing purpose 
	 *need to be improved*/
	memcpy(packet_out_buffer, &oh, sizeof(oh));
	memcpy(packet_out_buffer+sizeof(oh), &pkt, sizeof(pkt));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt), &oao, sizeof(oao));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt)+sizeof(oao), data, data_length);
	send(qn->sw->sockfd, &packet_out_buffer, data_length+16+8+sizeof(pkt), MSG_DONTWAIT);
}

static void port_desc_reply_handler(struct q_node* qn)
{
	char* pkt_ptr = qn->rx_packet + 16;
	uint8_t i = 0;
	uint16_t len = 16; //16 is the length of ofp header and multipart reply header
	while(len < qn->packet_length)
	{
		inverse_memcpy(&(qn->sw->ports[i++]), pkt_ptr, 64);
		len += 64; //64 is the length of the port structure
		pkt_ptr += 64;
	}
}

static uint32_t packet_in_msg_get_bufferid(struct q_node* qn)
{
	uint32_t buffer_id;
	inverse_memcpy(&buffer_id, qn->rx_packet + 8, 4);
	return buffer_id;
}

static uint16_t packet_in_msg_get_total_len(struct q_node* qn)
{
	uint16_t total_len;
	inverse_memcpy(&total_len, qn->rx_packet + 12, 2);
	return total_len;
}

static uint8_t packet_in_msg_get_reason(struct q_node* qn)
{
	uint8_t reason;
	memcpy(&reason, qn->rx_packet + 14, 1);
	return reason;
}

static uint8_t packet_in_msg_get_tableid(struct q_node* qn)
{
	uint8_t tableid;
	memcpy(&tableid, qn->rx_packet + 15, 1);
	return tableid;
}

static uint64_t packet_in_msg_get_cookie(struct q_node* qn)
{
	uint64_t cookie;
	inverse_memcpy(&cookie, qn->rx_packet + 16, 8);
	return cookie;
}

static void packet_in_msg_get_data(struct q_node* qn, char* buffer, uint16_t total_len)
{
	//uint16_t total_len = packet_in_msg_get_total_len(qn);
	//printf("packet_length:%d\n", qn->packet_length);
	//printf("total_len: %d\n", total_len);
	memcpy(buffer, qn->rx_packet + qn->packet_length - total_len, total_len);
	/*int i;
	for(i = 0; i < total_len; i++)
	{
		printf("0x%x ",*(char*)(buffer+i));
		if((i+1)%8==0)
			printf("\n");
	}
	printf("\n");*/
}

static void get_ether_src_mac(char * buffer)
{

}

static uint16_t get_ether_type(char * buffer)
{
	uint16_t ether_type;
	inverse_memcpy(&ether_type, buffer + 12, 2);
	return ether_type;
}

static void parse_ether_type(struct q_node* qn, char * buffer)
{
	uint16_t ether_type = get_ether_type(buffer);
	printf("ether_type: %d\n", ether_type);
	switch(ether_type)
	{
		case 0x806: arp_msg_handler(qn, buffer); break;
	}
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
			case 10: packet_in_msg_handler(qn); break;
			case 19: multipart_reply_handler(qn); break;
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
	if(qn->sw->is_feature_request_sent == 0)
	{
		send_switch_features_request(qn);
		qn->sw->is_feature_request_sent = 1;
	}
	if(qn->sw->is_port_desc_request_sent == 0)
	{
		send_multipart_port_desc_request(qn);
		qn->sw->is_port_desc_request_sent = 1;
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
	mf_write_socket_log("feature_reply Message received", qn->sw->sockfd);
	inverse_memcpy(&qn->sw->datapath_id, qn->rx_packet + 8, 8);
	inverse_memcpy(&qn->sw->n_buffers, qn->rx_packet + 16, 4);
	memcpy(&qn->sw->n_tables, qn->rx_packet + 20, 1);
	memcpy(&qn->sw->auxiliary_id, qn->rx_packet + 21, 1);
	inverse_memcpy(&qn->sw->capabilities, qn->rx_packet + 24, 4);
	printf("feature_reply message handling\n");
}

void packet_in_msg_handler(struct q_node* qn)
{
	printf("packet_in_msg_received\n");
	char buffer[1024];
	//bzero(buffer, 1024);
	//uint32_t buffer_id = packet_in_msg_get_bufferid(qn);
	//uint8_t reason = packet_in_msg_get_reason(qn);
	//uint8_t table_id = packet_in_msg_get_tableid(qn);
	//uint64_t cookie = packet_in_msg_get_cookie(qn);
	uint16_t total_len = packet_in_msg_get_total_len(qn);
	packet_in_msg_get_data(qn, buffer, total_len);
	parse_ether_type(qn, buffer);
}

void multipart_reply_handler(struct q_node* qn)
{
	uint16_t type;
	inverse_memcpy(&type, qn->rx_packet + 8, 2);
	if(type == 13)
		port_desc_reply_handler(qn);
}

void arp_msg_handler(struct q_node* qn, char* buffer)
{
	printf("arp msg received\n");
}
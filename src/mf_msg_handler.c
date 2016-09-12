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
#include "mf_devicemgr.h"
#include "mf_topomgr.h"
#include "dbg.h"

/*=====================================
Global variables
Use global variables as little as possible
For the performance's sake
in a multi thread environment
======================================*/

struct msg_handlers * MSG_HANDLERS;

extern 
struct mf_devicemgr MF_DEVICE_MGR;
/*=====================================
Function registers 
======================================*/

static void regist_msg_handler(struct single_msg_handler ** handler_list, struct single_msg_handler * handler)
{
	if(handler == NULL)
	{
		log_warn("handler is NULL");
		return;
	}
	struct single_msg_handler ** tmp = handler_list;
	if(*handler_list == NULL)
	{
		*handler_list = handler;
		return;
	}
	while((*tmp)->next)
		tmp = &(*tmp)->next;
	(*tmp)->next = handler;
}

static void unregister_msg_handler(struct single_msg_handler ** handler_list, msg_handler_func func)
{
	if(handler_list == NULL || func == NULL)
	{
		log_warn("Handler list is NULL or Func is NULL");
		return;
	}
	struct single_msg_handler ** tmp = handler_list;
	if(*handler_list == NULL)
	{
		log_warn("handler list is NULL");
		return;
	}
	while(*tmp)
	{
		if((*tmp)->handler_func == func)
		{
			if((*tmp)->next)
			{
				*tmp = (*tmp)->next;
			}
			else
			{
				(*tmp)->next = NULL;
			}
			free(*tmp);
			return;
		}
		tmp = &((*tmp)->next);
	}
}

void msg_handlers_init()
{
	MSG_HANDLERS = (struct msg_handlers *)malloc(sizeof(struct msg_handlers));
	memset(MSG_HANDLERS, 0 , sizeof(struct msg_handlers));
	regist_msg_handler(&MSG_HANDLERS->hello_msg_handler_list_head, single_msg_handler_create(hello_msg_handler));
	regist_msg_handler(&MSG_HANDLERS->echo_request_handler_list_head, single_msg_handler_create(echo_request_handler));
	regist_msg_handler(&MSG_HANDLERS->feature_reply_handler_list_head, single_msg_handler_create(feature_reply_handler));
	regist_msg_handler(&MSG_HANDLERS->packet_in_msg_handler_list_head, single_msg_handler_create(packet_in_msg_handler));
}

struct single_msg_handler * single_msg_handler_create(msg_handler_func func)
{
	if(func == NULL)
	{
		log_warn("func is NULL");
		return NULL;
	}
	struct single_msg_handler * handler = (struct single_msg_handler *) malloc(sizeof(*handler));
	if(handler == NULL)
	{
		log_err("malloc failed");
		exit(0);
	}
	handler->handler_func = func;
	handler->next = NULL;
	return handler;
}

void msg_handler_func_register(enum MSG_HANDLER_TYPE type, msg_handler_func func)
{
	struct single_msg_handler * handler = single_msg_handler_create(func);
	switch(type)
	{
		case HELLO_MSG_HANDLER_FUNC:
			regist_msg_handler(&MSG_HANDLERS->hello_msg_handler_list_head, handler);break;
		case ECHO_REQUEST_HANDLER_FUNC:
			regist_msg_handler(&MSG_HANDLERS->echo_request_handler_list_head, handler);break;
		case FEATURE_REPLY_HANDLER_FUNC:
			regist_msg_handler(&MSG_HANDLERS->feature_reply_handler_list_head, handler);break;
		case PACKET_IN_MSG_HANDLER_FUNC:
			regist_msg_handler(&MSG_HANDLERS->packet_in_msg_handler_list_head, handler);break;
		default:log_warn("wrong handler type"); break;
	}
}

void msg_handler_func_unregister(enum MSG_HANDLER_TYPE type, msg_handler_func func)
{
	if(func == NULL)
	{
		log_warn("func is NULL");
		return;
		//exit(0);
	}
	switch(type)
	{
		case HELLO_MSG_HANDLER_FUNC:
			unregister_msg_handler(&MSG_HANDLERS->hello_msg_handler_list_head, func);break;
		case ECHO_REQUEST_HANDLER_FUNC:
			unregister_msg_handler(&MSG_HANDLERS->echo_request_handler_list_head, func);break;
		case FEATURE_REPLY_HANDLER_FUNC:
			unregister_msg_handler(&MSG_HANDLERS->feature_reply_handler_list_head, func);break;
		case PACKET_IN_MSG_HANDLER_FUNC:
			unregister_msg_handler(&MSG_HANDLERS->packet_in_msg_handler_list_head, func);break;
		default: log_warn("wrong handler type"); break;
	}
}

static void msg_handler_exec(struct single_msg_handler * handler_head, struct q_node * qn)
{
	if(unlikely(handler_head == NULL))
	{
		log_warn("handler head is NULL");
		return;
	}
	struct single_msg_handler * tmp = handler_head;
	while(tmp)
	{
		tmp->handler_func(qn);
		tmp = tmp->next;
	}
}

/*=====================================
Functions for msg handlers
======================================*/

void hello_msg_stopwatch_callback(void* arg) //for timer function test
{
	if(arg == NULL)
	{
		log_warn("stopwatch callback arg is null");
		return;
	}
	else
	{
		//struct q_node * qn = (struct q_node *)arg;
		printf("Print this msg every 1 sec\n");
	}
}

static void send_switch_features_request(struct q_node* qn)
{
	uint32_t xid = generate_random();
	/*
		TODO:
			1.hide the xid to msg constructor
			2.what is the purpose of feature_request_xid in the sw struct?
	 */
	qn->sw->feature_request_xid = xid;
	struct ofp_header oh = of13_switch_feature_msg_constructor(xid);
	send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
	//mf_write_socket_log("Feature_request Message sent", qn->sw->sockfd);
}

void send_multipart_port_desc_request(struct q_node* qn)
{
	struct ofp_multipart_request omr = of13_multiaprt_request_constructor(13, 0);
	send(qn->sw->sockfd, &omr, sizeof(omr), MSG_DONTWAIT);
}

void send_packet_out(struct q_node* qn, uint32_t xid, uint32_t buffer_id, void* data, uint32_t data_length)
{
	char packet_out_buffer[1024];
	
	struct ofp11_packet_out pkt = of13_packet_out_msg_constructor(buffer_id, 16);
	struct ofp_header oh = ofp13_msg_header_constructor(xid, 13, data_length + 16 + 8 + sizeof(pkt));
	struct ofp_action_output oao = ofp13_action_output_constructor(1);
	/*TODO:
		 *quick code for testing purpose 
		 *need to be improved
	*/
	memcpy(packet_out_buffer, &oh, sizeof(oh));
	memcpy(packet_out_buffer+sizeof(oh), &pkt, sizeof(pkt));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt), &oao, sizeof(oao));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt)+sizeof(oao), data, data_length);
	send(qn->sw->sockfd, &packet_out_buffer, data_length+16+8+sizeof(pkt), MSG_DONTWAIT);
}

static void send_lldp_packet_out_per_port(struct mf_switch *sw, lldp_t * pkt, ovs_be32 port_no)
{
	char packet_out_buffer[1024];
	struct ofp11_packet_out pkt_out = of13_packet_out_msg_constructor(0, 16);
	struct ofp_header oh = ofp13_msg_header_constructor(0, 13, sizeof(*pkt) + 16 + 8 + sizeof(pkt_out));
	struct ofp_action_output oao = ofp13_action_output_constructor(port_no);
	memcpy(packet_out_buffer, &oh, sizeof(oh));
	memcpy(packet_out_buffer+sizeof(oh), &pkt_out, sizeof(pkt_out));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt_out), &oao, sizeof(oao));
	memcpy(packet_out_buffer+sizeof(oh)+sizeof(pkt_out)+sizeof(oao), pkt, sizeof(*pkt));
	send(sw->sockfd, &packet_out_buffer, sizeof(*pkt)+16+8+sizeof(pkt_out), MSG_DONTWAIT);
}

static void port_desc_reply_handler(struct q_node* qn)
{
	char* pkt_ptr = qn->rx_packet + 16;
	uint8_t i = 0;
	uint16_t len = 16; //16 is the length of ofp header and multipart reply header
	while(len < qn->packet_length)
	{
		//inverse_memcpy((void*)&(qn->sw->ports[i].port_no), pkt_ptr, 4);
		qn->sw->ports[i].port_no= ntoh_32bit(pkt_ptr);
		inverse_memcpy((void*)&(qn->sw->ports[i].hw_addr), pkt_ptr + 8, 6);
		i++;
		len += 64; //64 is the length of the port structure
		pkt_ptr += 64;
	}
	qn->sw->port_num = i;
}

static uint64_t get_src_mac_addr(char* data)
{
	uint64_t src_mac = 0;
	inverse_memcpy(&src_mac, data + 6, 6);
	return src_mac;
}

/*
	TODO :
		1. LLDP packet constructor --Done
		2. Send LLDP to sw_ports with timer --Done 
		3. LLDP msg handler -- Done
		4. Path calculate algorithm
*/

static void send_LLDP_packet(void * arg)
{
	struct mf_switch * sw = (struct mf_switch*)arg;
	lldp_t pkt;
	uint32_t j= 0;
	int k = 0;
	for(j = 0; j < MF_DEVICE_MGR.total_switch_number; j++)
	{
		int i = 0;
		sw = get_next_switch(&k);
		if(sw == NULL)
			break;
		for(; i < sw->port_num; i++)
		{
			create_lldp_pkt((void *)&(sw->ports[i].hw_addr), sw->datapath_id, sw->ports[i].port_no, &pkt);
			send_lldp_packet_out_per_port(sw, &pkt, sw->ports[i].port_no);
		}
	}
}

/*
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
	memcpy(buffer, qn->rx_packet + qn->packet_length - total_len, total_len);
}

static void get_ether_src_mac(char * buffer)
{

}

static void packet_in_msg_get_data(struct q_node* qn, char* buffer, uint16_t total_len)
{
	memcpy(buffer, qn->rx_packet + qn->packet_length - total_len, total_len);
}
static uint16_t get_ether_type(char * buffer)
{
	uint16_t ether_type;
	inverse_memcpy(&ether_type, buffer + 12, 2);
	return ether_type;
}
*/

static ovs_be32 packet_in_msg_get_in_port_num(struct q_node *qn)
{
	ovs_be32 port_num = ntoh_32bit(qn->rx_packet + 32);
	return port_num;  
}


static void parse_ether_type(struct q_node* qn, uint32_t xid, char * buffer, uint16_t total_len)
{
	if(unlikely(qn == NULL || buffer == NULL))
	{
		log_warn("qn is NULL or buffer is NULL");
		return;
	}
	uint16_t ether_type = ntoh_16bit(buffer + 12);
	switch(ether_type)
	{
		case 0x806: arp_msg_handler(qn, xid, buffer, total_len);break;
		case 0x88cc: lldp_msg_handler(qn, xid, buffer, total_len);break;
		default:log_warn("wrong ether type");log_info("ether type: %x\n", ether_type);break;
	}
}

/*=====================================
Msg handler functions
======================================*/


void msg_handler(uint8_t type, uint8_t version, struct q_node* qn)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	if(likely(version == 4))
	{
		switch(type)
		{
			case 0: msg_handler_exec(MSG_HANDLERS->hello_msg_handler_list_head, qn);break;
			case 2: echo_request_handler(qn); break;
			case 6: feature_reply_handler(qn); break;
			case 10: packet_in_msg_handler(qn); break;
			case 12: port_status_msg_handler(qn);break;
			case 19: multipart_reply_handler(qn); break;
			default: log_warn("Invalid msg type"); break;
		}
	}
	else
	{
		log_warn("Msg is not Openflow Version 1.3");
		log_warn("Msg version is: %d\n", version);
	}
}

void hello_msg_handler(struct q_node* qn)
{
	static int if_LLDP_timer_exist = 0;
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	static uint8_t is_timer_added;
	if(is_timer_added == 0)
	{
		//struct stopwatch * spw = stopwatch_create(1.0, &hello_msg_stopwatch_callback, PERMANENT, (void*)qn);
		is_timer_added = 1;
	}
	/*Log behavior leads to race condition when massive switches
	to connect at the same time*/
	//mf_write_socket_log("Hello Message received", qn->sw->sockfd);
	uint32_t xid = ntoh_32bit(qn->rx_packet + 4);
	struct ofp_header oh = of13_hello_msg_constructor(xid);
	if(qn->sw->is_hello_sent == 0)
	{
		send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
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
	if(if_LLDP_timer_exist == 0)
	{
		struct stopwatch * spw = stopwatch_create(1.0, &send_LLDP_packet, PERMANENT, (void*)(qn->sw));
		if_LLDP_timer_exist = 1;
	}
	log_info("Hello msg handling");
}

void echo_request_handler(struct q_node* qn)
{	
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	uint32_t xid = ntoh_32bit(qn->rx_packet + 4);
	struct ofp_header oh = of13_echo_reply_msg_constructor(xid);
	send(qn->sw->sockfd, &oh, sizeof(oh), MSG_DONTWAIT);
}


void feature_reply_handler(struct q_node* qn)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	log_info("feature_reply message handling");
	qn->sw->datapath_id = ntoh_64bit(qn->rx_packet + 8);
	qn->sw->n_buffers = ntoh_32bit(qn->rx_packet + 16);
	qn->sw->n_tables = *(char*)(qn->rx_packet + 20);
	qn->sw->auxiliary_id= *(char*)(qn->rx_packet + 21);
	qn->sw->capabilities = ntoh_32bit(qn->rx_packet + 24);
}

void packet_in_msg_handler(struct q_node* qn)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	uint32_t xid = ntoh_32bit(qn->rx_packet + 4);
	uint16_t total_len = ntoh_16bit(qn->rx_packet + 12);
	char * data_pointor = qn->rx_packet + qn->packet_length - total_len;
	parse_ether_type(qn, xid, data_pointor, total_len);
}

void port_status_msg_handler(struct q_node* qn)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	uint8_t reason = *(qn->rx_packet + 8);
	if(reason == 0)
	{
		pthread_mutex_lock(&(qn->sw->switch_mutex));
		char* pkt_ptr = qn->rx_packet + 16;
		uint8_t i = qn->sw->port_num;
		uint16_t len = 16; 
		while(len < qn->packet_length)
		{
			//inverse_memcpy((void*)&(qn->sw->ports[i].port_no), pkt_ptr, 4);
			qn->sw->ports[i].port_no = ntoh_32bit(pkt_ptr);
			inverse_memcpy((void*)&(qn->sw->ports[i].hw_addr), pkt_ptr + 8, 6);
			i++;
			len += 64; //64 is the length of the port structure
			pkt_ptr += 64;
		}
		qn->sw->port_num = i;
		pthread_mutex_unlock(&(qn->sw->switch_mutex));
		switch_print(qn->sw);
	}
}

void multipart_reply_handler(struct q_node* qn)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	uint16_t type = ntoh_16bit(qn->rx_packet + 8);
	if(type == 13)
		port_desc_reply_handler(qn);
}

void arp_msg_handler(struct q_node* qn, uint32_t xid, char* buffer, uint16_t total_len)
{
	if(unlikely(qn == NULL))
	{
		log_warn("qn is NULL");
		return;
	}
	uint64_t mac_addr = get_src_mac_addr(buffer);
	host_hash_value_add(qn->sw, 5, mac_addr);
	send_packet_out(qn, xid, 0, buffer, total_len);
}

static uint64_t get_dpid_from_LLDP_packet(char * buffer)
{
	uint64_t dpid = ntoh_64bit(buffer + 17);
	return dpid;	
}

static uint16_t get_outport_from_LLDP_packet(char * buffer)
{
	uint16_t outport = ntoh_16bit(buffer + 28);
	return outport;	
}

void lldp_msg_handler(struct q_node* qn, uint32_t xid, char* buffer, uint16_t total_len)
{
	ovs_be32 in_port_num = 0;	
	in_port_num = packet_in_msg_get_in_port_num(qn);
	struct ofp11_port * port = get_switch_port_by_port_num(qn->sw, in_port_num);
	if(unlikely(port == NULL))
	{
		log_warn("Bad Port");
		return;
	}
	struct link_node * right_node = link_node_create(qn->sw, port);
	uint64_t dpid = get_dpid_from_LLDP_packet(qn->rx_packet + qn->packet_length - total_len);
	struct mf_switch *sw = get_switch_by_dpid_from_list(dpid);
	if(unlikely(sw == NULL))
	{
		log_warn("Bad dpid, can Not get switch");
		return;
	}
	uint16_t outport = get_outport_from_LLDP_packet(qn->rx_packet + qn->packet_length - total_len);
	struct ofp11_port * port_out = get_switch_port_by_port_num(sw, outport);
	if(unlikely(port_out == NULL))
	{
		log_warn("Bad Port");
		return;
	}
	struct link_node * left_node = link_node_create(sw, port_out); 
	uint32_t rst = 0;
	struct network_link * netlink = network_link_create(left_node, right_node);
	if(netlink != NULL)
		rst = sw_link_insert(&(qn->sw->link_list), netlink);
	if(rst == 1)
		print_switch_link(qn->sw);
	//print_all_switches();
}

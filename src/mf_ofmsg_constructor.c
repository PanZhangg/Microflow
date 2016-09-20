#include "mf_ofmsg_constructor.h"
#include "mf_utilities.h"
#include <arpa/inet.h>
#include <strings.h>
//#include "openflow-common.h"

struct ofp_header ofp13_msg_header_constructor(uint32_t xid, uint8_t type, uint16_t length)
{
	struct ofp_header oh;
	oh.version = 0x04;
	oh.type = type;
	oh.length = htons(length);
	oh.xid = xid;
	return oh;
}

struct ofp_header of13_hello_msg_constructor(uint32_t xid)
{
	struct ofp_header of13_hello_msg;
	of13_hello_msg.version = 0x04;
	of13_hello_msg.type = 0x00;
	of13_hello_msg.length = htons(0x08);
	of13_hello_msg.xid = xid;
	return of13_hello_msg;
}

struct ofp_header of13_echo_reply_msg_constructor(uint32_t xid)
{
	struct ofp_header of13_echo_reply_msg;
	of13_echo_reply_msg.version = 0x04;
	of13_echo_reply_msg.type = 0x03;
	of13_echo_reply_msg.length = htons(0x08);
	of13_echo_reply_msg.xid = xid;
	return of13_echo_reply_msg;
}

struct ofp_header of13_switch_feature_msg_constructor()
{
	struct ofp_header of13_switch_feature_request_msg;
	of13_switch_feature_request_msg.version = 0x04;
	of13_switch_feature_request_msg.type = 0x05;
	of13_switch_feature_request_msg.length = htons(0x08);
	uint32_t xid = generate_random();
	of13_switch_feature_request_msg.xid = xid;
	return of13_switch_feature_request_msg;
}

struct ofp11_packet_out of13_packet_out_msg_constructor(uint32_t buffer_id, uint16_t actions_len)
{
	static struct ofp11_packet_out po;
	po.buffer_id = htonl(buffer_id);
	po.in_port = htonl(0xfffffffd); // OFPP_CONTROLLER
	po.actions_len = htons(actions_len);
	return po;
}

struct ofp_action_header ofp13_action_header_constructor(uint16_t type, uint16_t len)
{
	struct ofp_action_header oah;
	oah.type = type;
	oah.len = len;
	return oah;
}

struct ofp_action_output ofp13_action_output_constructor(uint32_t port)
{
	static struct ofp_action_output oao;
	oao.type = 0;
	oao.len = htons(16);
	oao.port = htonl(port);
	oao.max_len = 0;
	//bzero(&oao.pad, sizeof(oao.pad));
	return oao;
}


struct ofp_multipart_request of13_multiaprt_request_constructor(uint16_t type, uint16_t flags)
{
	static struct ofp_multipart_request omr;
	struct ofp_header oh;
	uint32_t xid = generate_random();
	oh = ofp13_msg_header_constructor(xid, 18, 16);
	omr.header = oh;
	omr.type = htons(type);
	omr.flags = htons(flags);
	return omr;
}

struct ofp11_flow_mod of13_flow_mod_msg_constructor(uint8_t table_id,
						    uint8_t command,
						    ovs_be16 hard_timeout, 
						    ovs_be16 idle_timeout, 
						    ovs_be16 priority,
						    ovs_be32 buffer_id,
						    ovs_be16 flags)
{
	struct ofp11_flow_mod flow_mod;
	flow_mod.cookie = 0;
	flow_mod.cookie_mask = 0;
	flow_mod.table_id = table_id;
	flow_mod.command = command;
	flow_mod.hard_timeout = hard_timeout;
	flow_mod.idle_timeout = idle_timeout;
	flow_mod.priority = priority;
	flow_mod.buffer_id = buffer_id;
	flow_mod.out_port = 0;
	flow_mod.out_group = 0;
	flow_mod.flags = flags;
	return flow_mod;
}

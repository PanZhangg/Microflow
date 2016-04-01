#include "mf_ofmsg_constructor.h"
#include <arpa/inet.h>
//#include "openflow-common.h"
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

struct ofp_header of13_switch_feature_msg_constructor(uint32_t xid)
{
	struct ofp_header of13_switch_feature_request_msg;
	of13_switch_feature_request_msg.version = 0x04;
	of13_switch_feature_request_msg.type = 0x05;
	of13_switch_feature_request_msg.length = htons(0x08);
	of13_switch_feature_request_msg.xid = xid;
	return of13_switch_feature_request_msg;
}
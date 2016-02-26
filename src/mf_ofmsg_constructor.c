#include "mf_ofmsg_constructor.h"
//#include "openflow-common.h"
struct ofp_header of13_hello_msg_constructor(uint32_t xid)
{
	struct ofp_header of13_hello_msg;
	of13_hello_msg.version = 0x04;
	of13_hello_msg.type = 0x00;
	of13_hello_msg.length = 0x08;
	of13_hello_msg.xid = xid;
	return of13_hello_msg;
}
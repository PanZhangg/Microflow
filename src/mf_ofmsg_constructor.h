#ifndef __MF_OFMSG_CONSTRUCTOR__
#define __MF_OFMSG_CONSTRUCTOR__

#include "./Openflow/openflow-common.h"
#include "./Openflow/openflow.h"

struct ofp_header ofp13_msg_header_constructor(uint32_t xid, uint8_t type, uint16_t length);
struct ofp_header of13_hello_msg_constructor(uint32_t xid);
struct ofp_header of13_echo_reply_msg_constructor(uint32_t xid);
struct ofp_header of13_switch_feature_msg_constructor(uint32_t xid);
struct ofp11_packet_out of13_packet_out_msg_constructor(uint32_t buffer_id, uint16_t actions_len);
struct ofp_action_header ofp13_action_header_constructor(uint16_t type, uint16_t len);
struct ofp_action_output ofp13_action_output_constructor(uint32_t port);
struct ofp_multipart_request of13_multiaprt_request_constructor(uint16_t type, uint16_t flags);


#endif

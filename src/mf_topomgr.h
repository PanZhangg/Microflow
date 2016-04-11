#ifndef MF_TOPOMGR_H__
#define MF_TOPOMGR_H__

#include "Openflow/types.h"
#include "./Openflow/openflow-1.1.h"

struct mf_switch;

#define MAX_NETWORK_LINK_NUM 4096

struct topo_link_node
{
	uint64_t switch_dpid;
	struct ofp11_port switch_port;
};

struct topo_link
{
	struct topo_link_node left_node;
	struct topo_link_node right_node;
};

struct network_topo
{
	topo_link[MAX_NETWORK_LINK_NUM];
};

struct topo_link_node topo_link_node_create(struct mf_switch* sw);
struct topo_link topo_link_create(struct topo_link_node left, struct topo_link_node right);
struct network_topo network_topo_create();
void push_link_to_network_topo(struct topo_link link, struct network_topo topo);

#endif

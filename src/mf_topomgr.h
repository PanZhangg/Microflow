#ifndef MF_TOPOMGR_H__
#define MF_TOPOMGR_H__

#include "Openflow/types.h"
#include "./Openflow/openflow-1.1.h"

struct mf_switch;

#define MAX_NETWORK_LINK_NUM 4096
#define MAX_NETWORK_LINK_NUM_PER_SWITCH 256 //identical to MAX SWITCH PORT NUM

struct topo_link_node
{
	uint64_t switch_dpid;
	//struct ofp11_port switch_port;
	uint32_t switch_port_no;
	uint8_t is_link_to_host;
};

extern struct topo_link_node TOPO_LINK_NODE_LIST[MAX_NETWORK_LINK_NUM * 2];

struct topo_link
{
	struct topo_link_node* original_node;
	struct topo_link_node* adjacent_node;
};
/*
struct links_of_switch
{
	uint64_t switch_dpid;
	struct topo_link[MAX_NETWORK_LINK_NUM_PER_SWITCH] links;
};
*/
/*
struct network_topo
{
	struct topo_link network_links[MAX_NETWORK_LINK_NUM];
};
*/

struct network_path
{
	uint16_t hop_num;
	struct topo_link * head;
	struct topo_link * next;
};

struct topo_link_node topo_link_node_create(struct mf_switch* sw);
struct topo_link topo_link_create(struct topo_link_node* , struct topo_link_node* );


#endif

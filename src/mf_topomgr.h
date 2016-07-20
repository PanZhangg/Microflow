#ifndef MF_TOPOMGR_H__
#define MF_TOPOMGR_H__

#include "Openflow/types.h"
#include "./Openflow/openflow-1.1.h"
#include <pthread.h>


struct mf_switch;

#define MAX_NETWORK_LINK_NUM 4096
#define MAX_NETWORK_LINK_NUM_PER_SWITCH 256 //identical to MAX SWITCH PORT NUM
#define LONGEST_PATH_LINK_NUM 64
#define LINK_NODE_NUM 64 


struct mf_topomgr
{
	uint64_t total_node_number;
	uint64_t node_cache_array_size;
	pthread_mutex_t topomgr_mutex;
	struct link_node * available_slot;
	struct link_node * used_slot;
	uint32_t next_available_index;
};

struct link_node
{
	struct mf_switch * sw;
	struct ofp11_port * port;
	struct link_node * next; //pointers for available/used slot bi-link list
	struct link_node * prev;
	uint8_t is_occupied;
};

struct network_link
{
	struct link_node* src;
	struct link_node* dst;
	uint8_t is_occupied;
	struct network_link * sw_link_next;
};

struct sw_link_list
{
	uint16_t link_num;
	struct network_link * head;
};

struct path_link_list
{
	uint16_t hop_num;
	struct network_link * path_link_list[LONGEST_PATH_LINK_NUM];
};

void mf_topomgr_create();
struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port);
struct network_link * network_link_create(struct link_node* src, struct link_node* dst);
struct path_link_list * path_link_list_create();
void sw_link_insert(struct sw_link_list * list, struct network_link * link);
void network_path_insert(struct path_link_list * list, struct network_link * link);
void sw_link_delete(struct sw_link_list * list, struct network_link * link);
void path_link_list_free(struct path_link_list * );
void network_link_free(struct network_link * link);
struct path_link_list * find_one_path_between_switches(struct mf_switch * src_sw, struct mf_switch * dst_sw);


#endif

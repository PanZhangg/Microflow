#ifndef MF_TOPOMGR_H__
#define MF_TOPOMGR_H__

#include "Openflow/types.h"
#include "./Openflow/openflow-1.1.h"
#include <pthread.h>


struct mf_switch;

#define MAX_NETWORK_LINK_NUM 4096
#define MAX_NETWORK_LINK_NUM_PER_SWITCH 256 //identical to MAX SWITCH PORT NUM
#define LONGEST_PATH_LINK_NUM 64


struct mf_topomgr
{
	uint64_t total_node_number;
	uint64_t node_cache_array_size;
	pthread_mutex_t devicemgr_mutex;
	struct link_node * available_slot;
	struct link_node * used_slot;
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
};

struct link_list_element
{
	struct network_link * link;
	struct link_list_element * next;
};

struct sw_link_list
{
	uint16_t link_num;
	struct link_list_element * head;
};

struct path_link_list
{
	uint16_t hop_num;
	struct link_list_element * head;
};

void mf_topomgr_create();
struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port);
struct network_link * network_link_create(struct link_node* src, struct link_node* dst);
struct link_list_element * link_list_element_create(struct network_link * link);
struct sw_link_list * sw_link_list_create();
struct path_link_list * path_link_list_create();
void sw_link_insert(struct sw_link_list * list, struct link_list_element * link);
void network_path_insert(struct path_link_list* list, struct link_list_element * link);
void sw_link_delete(struct sw_link_list * list, struct link_list_element* link);
void path_link_delete(struct path_link_list * list, struct link_list_element* link);
void network_link_free(struct network_link * link);
void link_node_free(struct link_node * node);
void link_list_element_free(struct link_list_element* link);


#endif

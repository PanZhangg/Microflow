#include "mf_topomgr.h"
#include <stdlib.h>

struct link_node * link_node_create(struct mf_switch* sw, struct ofp11_port* port)
{
	struct link_node * node = (struct link_node*)malloc(sizeof(*node));
	node->sw = sw;
	node->port = port;
	return node;
}

struct network_link * network_link_create(struct link_node* src, struct link_node* dst)
{
	struct network_link * link = (struct network_link *)malloc(sizeof(*link));
	link->src = src;
	link->dst = dst;
	return link;
}

struct link_list_element * link_list_element_create(struct network_link * link)
{
	struct link_list_element * link_element = (struct link_list_element *)malloc(sizeof(*link));
	link_element->link = link;
	link_element->next = NULL;
	return link_element;
}

struct sw_link_list * sw_link_list_create()
{
	struct sw_link_list * list = (struct sw_link_list *)malloc(sizeof(*list));
	list->link_num = 0;
	list->head = NULL;
	return list;
}

struct path_link_list * path_link_list_create()
{
	struct path_link_list * list = (struct path_link_list *)malloc(sizeof(*list));
	list->hop_num = 0;
	list->head = NULL;
	return list;
}

void sw_link_insert(struct sw_link_list * list, struct link_list_element * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->link_num == 0 && list->head == NULL)
	{
		list->head = link;
	}
	else
	{
		struct link_list_element * tmp = list->head;
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = link;
	}
	list->link_num++;
}

void network_path_insert(struct path_link_list * list, struct link_list_element * link)
{
	if(list == NULL || link == NULL)
		return;
	if(list->hop_num == 0 && list->head == NULL)
	{
		list->head = link;
	}
	else
	{
		struct link_list_element * tmp = list->head;
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = link;
	}
	list->hop_num++;
}

void network_link_free(struct network_link * link)
{
	if(link)
		free(link);
}

void link_node_free(struct link_node * node)
{
	if(node)
		free(node);
}
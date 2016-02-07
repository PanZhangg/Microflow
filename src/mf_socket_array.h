#ifndef __MF_SOCKET_ARRAY_H__
#define __MF_SOCKET_ARRAY_H__
#include "mf_socket.h"
#define SOCKET_HAS_DATA_LENGTH 256

//struct mf_socket;


struct mf_socket_array_node{
	struct mf_socket s;
	struct mf_socket_array_node * next;
	struct mf_socket_array_node * previous;
};


struct mf_socket_array{
	uint32_t array_length;
	struct mf_socket_node* head;
	struct mf_socket_node* tail;
	//struct mf_socket* node_has_data[SOCKET_HAS_DATA_LENGTH];
};

struct mf_socket_has_data_array{
	uint32_t has_data_array_length;
	struct mf_socket_node* head;
	struct mf_socket_node* tail;
};


struct mf_socket_array_node mf_socket_array_node_init(struct mf_socket s);
struct mf_socket_array mf_socket_array_init();

#endif
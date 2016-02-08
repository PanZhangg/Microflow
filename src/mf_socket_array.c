#include "mf_socket_array.h"
#include "stdlib.h"


struct mf_socket_array_node* mf_socket_array_node_init(struct mf_socket s){
	struct mf_socket_array_node* n = (struct mf_socket_array_node*)malloc(sizeof(*n));
	n->s = s;
	n->next_node = NULL;
	n->previous_node = NULL;
	return n;
}
struct mf_socket_array* mf_socket_array_init(){
	struct mf_socket_array* a = (struct mf_socket_array*)malloc(sizeof(*a));
	a->array_length = 0;
	a->head = NULL;
	a->tail = NULL;
	return a;
}

uint8_t push_mf_socket_array (struct mf_socket_array_node* n, struct mf_socket_array* q){
	if(n == NULL || q == NULL)
		return 0;
	if(!q->array_length){
		q->head = n;
		q->tail = n;
		q->array_length = 1;
		return 1;
	}else{
		q->tail->next_node = n;
		n->previous_node = q->tail;
		n->next_node = NULL;
		q->tail = n;
		q->array_length++;
		return 1;
	}
}
struct mf_socket_array_node* pop_mf_socket_array(struct mf_socket_array* q){
	if(!q->array_length)
		return NULL;
	else{
		struct mf_socket_array_node* tmp = NULL;
		tmp = q->head;
		q->head = tmp->next_node;
		q->head->previous_node = NULL;
		tmp->next_node = NULL;
		q->array_length--;
		return tmp;
	}
}
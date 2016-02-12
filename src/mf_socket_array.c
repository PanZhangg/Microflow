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

uint8_t insert_mf_socket_array (struct mf_socket_array_node* n, struct mf_socket_array* q){
	static previous_socket_fd;
	if(n == NULL || q == NULL)
		return 0;
	if(!q->array_length){
		q->head = n;
		q->tail = n;
		q->array_length = 1;
		previous_socket_fd = n->s.socket_fd;
		return 1;
	}else{
		if(n->s.socket_fd > previous_socket_fd){
			q->tail->next_node = n;
			n->previous_node = q->tail;
			n->next_node = NULL;
			q->tail = n;
			q->array_length++;
			previous_socket_fd = n->s.socket_fd;
			return 1;
		}else{
			struct mf_socket_array_node* tmp;
			tmp = q->head;
			while(tmp){
				if(n->s.socket_fd < tmp->s.socket_fd){
					n->next_node = tmp;		
					if(tmp->previous_node){ //tmp is not head
						tmp->previous_node->next_node = n;
						tmp->previous_node = n;
						n->previous_node = tmp->previous_node;
					}else{
						tmp->previous_node = n;
						q->head = n;
					}
					q->array_length++;
					break;
				}else
					tmp = tmp->next_node;
			}
		}
	}
}

void delet_socket_array_node(struct mf_socket_array_node* n, struct mf_socket_array* q){
	if(n == NULL || q == NULL)
		return 0;
	struct mf_socket_array_node* tmp;
	tmp = q->head;
	while(tmp){
		if(tmp.s->socket_fd < n.s->socket_fd){
			tmp = tmp->next;
			continue;
		}
		if(tmp.s->socket_fd == n.s->socket_fd){
			if(tmp->previous_node == NULL){

			} //tmp is head
		}
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
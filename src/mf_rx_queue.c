#include "mf_rx_queue.h"
#include <stdio.h>
#include <stdlib.h>

struct q_node* q_node_init(void* packet_buffer, uint16_t length, uint32_t socket_fd){
	struct q_node* q = (struct q_node*)malloc(sizeof(struct q_node));
	q->rx_packet = packet_buffer;
	q->packet_length = length;
	q->socket_fd = socket_fd;
	q->priority = 0;
	q->next_node = NULL;
	q->previous_node = NULL;
	return q;
}

struct mf_rx_queue* mf_rx_queue_init(){
	struct mf_rx_queue* q = (struct mf_rx_queue*)malloc(sizeof(struct mf_rx_queue));
	if(q == NULL){
		printf("\nmalloc memory failed\n");
		return NULL;
	}else{
	q->queue_length = 0;
	q->head = NULL;
	q->tail = NULL;
	pthread_mutex_init(&(q->q_mutex), NULL);
	return q;
	}
}

uint8_t push_q_node(struct q_node* n, struct mf_rx_queue* q){
	pthread_mutex_lock(&(q->q_mutex));
	if(n == NULL || q == NULL)
		return 0;
	if(!q->queue_length){
		q->head = n;
		q->tail = n;
		q->queue_length = 1;
		pthread_mutex_unlock(&(q->q_mutex));
		return 1;
	}
	else if(q->queue_length > 0)
	{
		q->tail->next_node = n;
		n->previous_node = q->tail;
		n->next_node = NULL;
		q->tail = n;
		q->queue_length++;
		pthread_mutex_unlock(&(q->q_mutex));
		return 1;
	}
	return 0;
}

struct q_node* pop_q_node(struct mf_rx_queue* q){
	pthread_mutex_lock(&(q->q_mutex));
	if(!q->queue_length || q->head == NULL)
	{
		pthread_mutex_unlock(&(q->q_mutex));
		return NULL;
	}
	else
	{
		struct q_node* tmp = NULL;
		tmp = q->head;
		if(q->queue_length == 1)
		{
			q->head = NULL;
			q->tail = NULL;
			q->queue_length = 0;
		}
		else if(q->queue_length > 1)
		{	
			q->head = tmp->next_node;
			q->head->previous_node = NULL;
			tmp->next_node = NULL;
			q->queue_length--;
		}
		else
		{
			pthread_mutex_unlock(&(q->q_mutex));
			return NULL;
		} 
		pthread_mutex_unlock(&(q->q_mutex));
		return tmp;
	}
}

void destory_q_node(struct q_node* n){
	if(n != NULL)
		free(n);
}

void destory_queue(struct mf_rx_queue* q){
	pthread_mutex_lock(&(q->q_mutex));
	if(q != NULL && q->head != NULL)
	{
		while(q->head)
		{
				struct q_node* tmp;
				tmp = q->head;
				q->head = tmp->next_node;
				destory_q_node(tmp);
		}
		q->queue_length = 0;
		pthread_mutex_unlock(&(q->q_mutex));
	}
	else
	{
		pthread_mutex_unlock(&(q->q_mutex));
	}
	free(q);
}

static void print_q_node(struct q_node* n){
	char* content = (char*)(n->rx_packet);
	printf("Packet content:%s\n", content);
}

void print_queue(struct mf_rx_queue* q){
	struct q_node* tmp;
	tmp = q->head;
	while(tmp){
		print_q_node(tmp);
		tmp = tmp->next_node;
	}

}


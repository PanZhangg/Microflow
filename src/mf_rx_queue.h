#ifndef __MF_RX_QUEUE_H__
#define __MF_RX_QUEUE_H__

#include "./Openflow/types.h"
#include <pthread.h>

struct mf_switch;

struct q_node{
	void* rx_packet;
	uint16_t packet_length;
	struct mf_switch* sw;
	struct q_node* next_node;
	struct q_node* previous_node;
};

struct mf_rx_queue{
	uint32_t queue_length;
	pthread_mutex_t q_mutex;
	struct q_node* head;
	struct q_node* tail;
};

struct q_node* q_node_init(void* packet_buffer, uint16_t length, struct mf_switch* sw);
struct mf_rx_queue* mf_rx_queue_init();
uint8_t push_q_node(struct q_node* n, struct mf_rx_queue* q);
struct q_node* pop_q_node(struct mf_rx_queue*q );
void destory_q_node(struct q_node* n);
void destory_queue(struct mf_rx_queue* q);
void print_queue(struct mf_rx_queue* q);



#endif

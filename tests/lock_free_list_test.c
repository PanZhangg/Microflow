#include "../src/mf_lf_list.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
struct list_node
{
	int val;
	struct lf_list list;
};

struct list_node node_head = {888, NULL};

void print_list(struct lf_list* head)
{
	struct lf_list* tmp = head;
	printf("Print func\n");
	while(tmp)
	{
		printf("val:%d\n",*(int*)((char*)tmp - 8));	
		printf("memory address: %x\n",(char)(tmp));
		tmp = tmp->next;
	}
	
}
void * thread_func(void * arg)
{
	printf("Thread starts\n");
	struct list_node * n = (struct list_node*)arg;
	printf("input memory address: %x\n",(char)&n->list);
	lf_list_insert(&(n->list), &(node_head.list));
	return NULL;
}

int main()
{
	struct list_node node1 = {1, NULL};
	struct list_node node2 = {2, NULL};
	struct list_node node3 = {3, NULL};
	struct list_node node4 = {4, NULL};
	struct list_node node5 = {5, NULL};
	struct list_node nodes[5] = {node1, node2, node3, node4, node5};
	pthread_t p[5];
	int i = 0;
	for(i = 0; i < 5; i++)	
	{
		pthread_t p[i];
		pthread_create(&p[i], NULL, thread_func, &nodes[i]);
	}
	for(i = 0; i < 5; i++)
	{
		pthread_join(p[i],NULL);
		printf("Thread: %d ends\n",i);
	}
	print_list(&(node_head.list));
	return 0;
}

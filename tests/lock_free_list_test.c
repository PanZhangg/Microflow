#include "../src/mf_lf_list.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
struct list_node
{
	int val;
	struct lf_list list;
};

struct list_node node_head = {888, {NULL, 0}};
int compare_list_node(void * arg, struct lf_list* n)
{
	struct list_node *tmp =(struct list_node *)arg;
	//printf("tmp->val:%d\n", tmp->val);
	struct list_node * pln = container_of(n, struct list_node,list);
	//printf("pln->val:%d\n", pln->val);
	if(tmp->val == pln->val)
	{
		printf("val found : %d\n", tmp->val);
		return 1;
	}
	else
		return 0;
}

void print_list(struct lf_list* head)
{
	struct lf_list* tmp = head;
	//printf("Print func\n");
	while(tmp)
	{
		//printf("offset: %d\n", (int)OFFSETOF(struct list_node, list));
		struct list_node * ptr = container_of(tmp, struct list_node, list);
		printf("val:%d\n",*(int*)((char*)tmp - OFFSETOF(struct list_node, list)));
		printf("val from ptr:%d\n", ptr->val);
		//printf("memory address: %x\n",(char)(tmp));
		tmp = tmp->next;
	}
	
}
void * thread_func(void * arg)
{
	//printf("Thread starts\n");
	struct list_node * n = (struct list_node*)arg;
	//printf("input memory address: %x\n",(char)&n->list);
	lf_list_insert(&(n->list), &(node_head.list));
	return NULL;
}

void * thread_func_pop(void * arg)
{
	printf("pop_Thread starts\n");
	struct lf_list * n = lf_list_pop(&(node_head.list));
	struct list_node * ptr;
	printf("val:%d\n",*(int*)((char*)n - OFFSETOF(struct list_node, list)));
	return NULL;

}

void * thread_func_delete(void * arg)
{
	struct list_node * n = (struct list_node *)arg;
	struct lf_list* t = lf_list_delete(&(n->list), &(node_head.list));
	if(t != NULL)
		printf("delete val:%d\n",*(int*)((char*)t - OFFSETOF(struct list_node, list)));
	return NULL;
}

void * thread_func_search(void *arg)
{
	lf_list_search_node(&(node_head.list),compare_list_node, arg);
}
int main()
{
	struct list_node node1 = {1, {NULL,0}};
	struct list_node node2 = {2, {NULL,0}};
	struct list_node node3 = {3, {NULL,0}};
	struct list_node node4 = {4, {NULL,0}};
	struct list_node node5 = {5, {NULL,0}};
	struct list_node nodes[5] = {node1, node2, node3, node4, node5};
	pthread_t p[10];
	int i = 0;
	/*for(i = 0; i < 5; i++)	
	{
		pthread_create(&p[i], NULL, thread_func, &nodes[i]);
	}
	for(i = 0; i < 5; i++)
	{
		pthread_join(p[i],NULL);
		printf("Thread: %d ends\n",i);
	}
	print_list(&(node_head.list));
/*
	for(i = 5; i < 7; i++)	
	{
		pthread_create(&p[i], NULL, thread_func_pop, &nodes[i-5]);
	}
	for(i = 5; i < 7; i++)
	{
		pthread_join(p[i],NULL);
		printf("Thread: %d ends\n",i);
	}
*/
//	print_list(&(node_head.list));
	for(i = 0; i < 10; i++)
	{
		if(i <  5)
		pthread_create(&p[i], NULL, thread_func, &nodes[i]);
		if(i <  7 && i >= 5)
		pthread_create(&p[i], NULL, thread_func_pop, &nodes[i-5]);
		if(i>=7 && i< 8)
		pthread_create(&p[i], NULL, thread_func_delete, &nodes[i-5]);
		if(i == 8 || i == 9)
		pthread_create(&p[i], NULL, thread_func_search, &nodes[i - 7]);
	}
	for(i = 0; i < 10; i++)
	{
		pthread_join(p[i],NULL);
		printf("Thread: %d ends\n",i);
	}
	print_list(&(node_head.list));
	return 0;
}

#include "mf_lf_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
void lf_list_insert(struct lf_list* i, struct lf_list* l)
{
	struct lf_list * tmp;
	do
	{
		tmp = (l->next);
		i->next = tmp;
		//struct lf_list * tmp = l;
	}while(!__sync_bool_compare_and_swap(&(l->next), tmp, i));
	//i->next = tmp;
}

struct lf_list * lf_list_pop(struct lf_list *l)
{
	struct lf_list * tmp;
	do
	{
 		tmp = l->next; 
	}while(!__sync_bool_compare_and_swap(&(l->next), tmp, tmp->next));
	return tmp;
}


struct lf_list * lf_list_delete(struct lf_list* i, struct lf_list* l)
{
	struct lf_list * tmp; 
	struct lf_list ** tmpp; 
again:
	do
	{
		tmp = l->next;
		tmpp = &(l->next);
		//printf("tmp is %x\n", (unsigned int)tmp);
		//printf("i is %x\n", (unsigned int)i);
		while(tmp) 
		{
			if(tmp == i)
			{
				//printf("tmp is %x\n", (unsigned int)tmp);
				//printf("i is %x\n", (unsigned int)i);
				//printf("*tmpp is %x\n", (unsigned int)*tmpp);
				tmp->mark = 1;
				break;
			}
			else
			{
				tmpp = &(tmp->next);
				tmp = tmp->next;
				//printf("tmp is %x\n", (unsigned int)tmp);
			}
		}
		if(tmp == NULL)
		{
			perror("List node has already been deleted\n");
			return NULL;
		}
		if((*((char*)tmpp + 8)) == 0x1)
		{
			printf("*tmpp :%d\n", (unsigned int)*tmpp);
			goto again;
		}
	}while(!__sync_bool_compare_and_swap(tmpp, i, i->next));
	tmp->mark = 0;
	return i;
}


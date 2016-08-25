#include "mf_lf_list.h"
#include <stdlib.h>
void lf_list_insert(struct lf_list* i, struct lf_list* l)
{
	struct lf_list * tmp;
	do
	{
		tmp = (l->next);
		//struct lf_list * tmp = l;
	}while(!__sync_bool_compare_and_swap(&(l->next), tmp, i));
	i->next = tmp;
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
	do
	{
		tmp = l;
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
	}while(!__sync_bool_compare_and_swap(tmpp, i, i->next));
	__sync_lock_release(tmpp);
	return tmp;
}


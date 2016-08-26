#include "mf_lf_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
void lf_list_insert(struct lf_list* i, struct lf_list* l)
{
	i->next = NULL;
	struct lf_list * tmp;
	do
	{
		tmp = (l->next);
		i->next = tmp;
	}while(!__sync_bool_compare_and_swap(&(l->next), tmp, i));
}

struct lf_list * lf_list_pop(struct lf_list *l)
{
	struct lf_list * tmp;
	do
	{
 		tmp = l->next; 
		if(tmp == NULL)
			return NULL;
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
		while(tmp) 
		{
			if(tmp == i)
			{
				tmp->mark = 1;
				break;
			}
			else
			{
				tmpp = &(tmp->next);
				tmp = tmp->next;
			}
		}
		if(tmp == NULL)
		{
			perror("List node has already been deleted");
			return NULL;
		}
		if((*((char*)tmpp + (int)OFFSETOF(struct lf_list, mark))) == 0x1)
		{
			goto again;
		}
	}while(!__sync_bool_compare_and_swap(tmpp, i, i->next));
	tmp->mark = 0;
	return i;
}

struct lf_list * lf_list_search_node(struct lf_list* l, compare cmp, void* arg)
{
	struct lf_list * tmp = l->next;
	while(tmp)
	{
		if(tmp->mark == 1)//is being deleted
		{
			l->mark = 0;
			return NULL;
  		}
		if(cmp(arg, tmp) == 1)	
		{
 			l->mark = 0;
			return tmp; 
		}
		tmp = tmp->next;
	}
	return NULL ;
}

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

void lf_list_delete(struct lf_list* i, struct lf_list* l)
{
	struct lf_list ** tmpp;
	do
	{
		struct lf_list * tmp = l;
		tmpp = &(l->next);
		while(tmp) 
		{
			if(tmp == i)
				break;
			else
			{
				tmpp = &(tmp->next);
				tmp = tmp->next;
			}
		}
		if(tmp == NULL)
			return;
	}while(!__sync_bool_compare_and_swap(tmpp, i, i->next));
}


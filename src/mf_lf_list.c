#include "mf_lf_list.h"
void lf_list_insert(struct lf_list* i, struct lf_list* l)
{
	do
	{
		struct lf_list * old_head = l;
		struct lf_list * tmp = l->next;
	}while(!__sync_bool_compare_and_swap(l->next, tmp, i));
	i->next = old_head;
}

void lf_list_delete(struct lf_list* i, struct lf_list* l)
{
	do
	{
		struct lf_list * tmp = l;
		struct lf_list ** tmpp = &(l->next)
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
	}while(!__sync_bool_compare_and_swap(*tmpp, i, i->next));
}


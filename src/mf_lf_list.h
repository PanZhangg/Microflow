#ifndef MF_LF_LIST_H__
#define MF_LF_LIST_H__
struct lf_list
{
	struct lf_list * next;  
}

void lf_list_insert(struct lf_list* i, struct lf_list* l);
void lf_list_delete(struct lf_list* i, struct lf_list* l);
#endif

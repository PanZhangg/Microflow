#include "mf_timer.h"
#include <stdlib.h>
#include <stdio.h>

struct stopwatch * stopwatch_create(uint8_t sec, stopwatch_switch_callback swc)
{
	struct stopwatch * stopwatch = (struct stopwatch*)malloc(sizeof(*stopwatch));
	stopwatch->stop_sec = sec;
	stopwatch->next = NULL;
	stopwatch->sw_callback = swc;
	return stopwatch;
}

struct stopwatch_list * stopwatch_list_create()
{
	struct stopwatch_list * swl = (struct stopwatch_list*)malloc(sizeof(*swl));
	swl->head = NULL;
	swl->stopwatch_num = 0;
	return swl;
}

void stopwatch_list_destory(struct stopwatch_list* swl)
{
	if(swl == NULL)
		return;
	if(swl->head == NULL)
	{
		free(swl);
		return;
	}
	else
	{
		
		while(swl->head)
		{
			struct stopwatch * tmp = swl->head;
			swl->head = tmp->next;
			free(tmp);
		}
		free(swl);
	}
}


void insert_stopwatch(struct stopwatch* stopwatch, struct stopwatch_list* swl)
{
	if(swl->head == NULL)
	{
		swl->head = stopwatch;
		return;
	}
	else
	{
		struct stopwatch * tmp = swl->head;
		while(tmp->next)
			tmp = tmp->next;
		tmp->next = stopwatch;
	}

}

void stopwatch_countdown(struct mf_switch * sw, struct stopwatch_list * swl)
{
	if(sw == NULL || swl->head == NULL)
		return;
	struct stopwatch * tmp;
	tmp = swl->head;
	while(tmp)
	{
		if(tmp->stop_sec-- == 0 && tmp->sw_callback)
		{
			tmp->sw_callback(sw);
		}
		tmp = tmp->next;
	}
	sleep(1);
}

char* get_asctime()
{
	char* asc_time = NULL;
	//time_t t = time(NULL);
	time_t t;
	if(time(&t) < 0)
	{
		printf("get time error\n");
	}
	asc_time = ctime(&t);
	char* tmp = asc_time;
	while(tmp)
	{
		if(*tmp == '\n')
		{
			*tmp = '\0';
			break;
		}
		tmp++;
	}
	//printf("\n%s",asc_time);
	return asc_time;
}
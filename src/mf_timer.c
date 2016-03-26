#include "mf_timer.h"
#include "mf_rx_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


struct stopwatch_list * MF_STOPWATCH_LIST;

struct stopwatch * stopwatch_create(float sec, stopwatch_switch_callback swc, enum STOPWATCH_TYPE type, void* callback_arg)
{
	struct stopwatch * stopwatch = (struct stopwatch*)malloc(sizeof(*stopwatch));
	stopwatch->stop_sec = sec;
	stopwatch->time_remain = sec;
	stopwatch->type = type;
	stopwatch->next = NULL;
	stopwatch->sw_callback = swc;
	stopwatch->callback_arg = callback_arg;
	insert_stopwatch(stopwatch, MF_STOPWATCH_LIST);
	return stopwatch;
}

struct stopwatch_list * stopwatch_list_create()
{
	struct stopwatch_list * swl = (struct stopwatch_list*)malloc(sizeof(*swl));
	swl->head = NULL;
	swl->stopwatch_num = 0;
	pthread_mutex_init(&swl->stopwatch_mutex, NULL);
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
	pthread_mutex_lock(&swl->stopwatch_mutex);
	if(swl->head == NULL)
	{
		swl->head = stopwatch;
		pthread_mutex_unlock(&swl->stopwatch_mutex);
		return;
	}
	else
	{
		struct stopwatch * tmp = swl->head;
		while(tmp->next)
			tmp = tmp->next;
		tmp->next = stopwatch;
	}
	pthread_mutex_unlock(&swl->stopwatch_mutex);
}

void stopwatch_countdown(struct stopwatch_list * swl)
{
	//printf("stopwatch countdown\n");
	pthread_mutex_lock(&swl->stopwatch_mutex);
	if(swl->head == NULL)
	{
		pthread_mutex_unlock(&swl->stopwatch_mutex);
		usleep(100000);//0.1 second
		return;
	}
	struct stopwatch * tmp;
	tmp = swl->head;
	while(tmp)
	{
		if(tmp->time_remain <= 0.0)
		{
			printf("countdown to zero\n");
			tmp->sw_callback(tmp->callback_arg);
			tmp->time_remain = tmp->stop_sec;
		}
		else
		{
			//printf("countdown by 0.1 seconds\n");
			tmp->time_remain = tmp->time_remain - 0.1;
		}
		if(tmp->type == DISPOSIBILE)
		{
			//delete this stopwatch
		}
		tmp = tmp->next;
	}
	pthread_mutex_unlock(&swl->stopwatch_mutex);
	usleep(100000);//0.1 second
}


void* stopwatch_worker(void* arg)
{
	//printf("stopwatch worker starts\n");
	//struct q_node * qn = (struct q_node*)arg;
	while(1)
	{
		stopwatch_countdown(MF_STOPWATCH_LIST);
	}
}

void start_stopwatch_thread(struct q_node * qn)
{
	MF_STOPWATCH_LIST = stopwatch_list_create();
	pthread_t thread_id;
	if((pthread_create(&thread_id, NULL, stopwatch_worker, NULL)) < 0)
	{
		printf("thread create error\n");
	}
	pthread_detach(thread_id);
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
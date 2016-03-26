#ifndef __MF_TIMER_H__
#define __MF_TIMER_H__

#include <time.h>
//#include "mf_switch.h"
#include "Openflow/types.h"

struct mf_switch;

enum  STOPWATCH_TYPE
{
	DISPOSIBILE,
	PERMANENT
};

typedef void (*stopwatch_switch_callback)(void* );

struct stopwatch
{
	float stop_sec;
	float time_remain;
	enum STOPWATCH_TYPE type;
	struct stopwatch * next;
	stopwatch_switch_callback sw_callback;
	void* callback_arg;
};

struct stopwatch_list
{
	struct stopwatch * head;
	uint8_t stopwatch_num;
	pthread_mutex_t stopwatch_mutex;
};

extern struct stopwatch_list * MF_STOPWATCH_LIST;

struct stopwatch * stopwatch_create(float sec, stopwatch_switch_callback,enum STOPWATCH_TYPE, void*);
struct stopwatch_list * stopwatch_list_create();
void insert_stopwatch(struct stopwatch* stopwatch, struct stopwatch_list* swl);
void stopwatch_list_destory(struct stopwatch_list*);
void stopwatch_countdown(struct stopwatch_list *);
void start_stopwatch_thread();


char* get_asctime();

#endif

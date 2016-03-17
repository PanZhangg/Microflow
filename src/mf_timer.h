#ifndef __MF_TIMER_H__
#define __MF_TIMER_H__

#include <time.h>
//#include "mf_switch.h"
#include "Openflow/types.h"

struct mf_switch;

typedef void (*stopwatch_switch_callback)(struct mf_switch* );

struct stopwatch
{
	uint8_t stop_sec;
	struct stopwatch * next;
	stopwatch_switch_callback sw_callback;
};

struct stopwatch_list
{
	struct stopwatch * head;
	uint8_t stopwatch_num;
};


struct stopwatch * stopwatch_create(uint8_t sec, stopwatch_switch_callback);
struct stopwatch_list * stopwatch_list_create();
void insert_stopwatch(struct stopwatch* stopwatch, struct stopwatch_list* swl);
void stopwatch_list_destory(struct stopwatch_list*);
void stopwatch_countdown(struct mf_switch *sw, struct stopwatch_list *);


char* get_asctime();

#endif

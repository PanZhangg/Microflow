#include "../src/mf_timer.h"


int main()
{
	struct stopwatch * spw1 = stopwatch_create(5, NULL);
	struct stopwatch * spw2 = stopwatch_create(6, NULL);
	struct stopwatch * spw3 = stopwatch_create(7, NULL);
	struct stopwatch_list * swl = stopwatch_list_create();
	insert_stopwatch(spw1,swl);
	insert_stopwatch(spw2,swl);
	insert_stopwatch(spw3,swl);
	stopwatch_countdown(NULL, swl);
	stopwatch_list_destory(swl);
	return 0;

}

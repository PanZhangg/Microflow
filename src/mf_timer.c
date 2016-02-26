#include "mf_timer.h"
#include <stdlib.h>
#include <stdio.h>

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
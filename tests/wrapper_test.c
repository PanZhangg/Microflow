#include "../src/mf_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
	time_t timestart, timeend;
	char * src = "abcd";
	char dst[5];
	dst[4] = '\0';
	set_CPU_instruction();
	int i = 0;
	/*time(&timestart);
	for(; i < 100000000; i++)
		memcpy((char*)dst, src, 4);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));*/
	int j = 0;
	time(&timestart);
	for(j= 0; j < 2; j++)
		mf_mov16((uint8_t*)dst, (const uint8_t*)src);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));
	printf("dst:%s\n",(char *)dst);
	return 0;
}


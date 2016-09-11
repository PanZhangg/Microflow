#include "../src/mf_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
	time_t timestart, timeend;
	char * src = "abcd";
	char * src1 = "efgh";
	char * src2 = "wxyz";
	char src_block[256];
	char dst[5];
	dst[4] = '\0';
	char dst1[5];
	dst1[4] = '\0';
	char dst2[5];
	dst2[4] = '\0';
	char dst3[256];
	set_CPU_instruction();
	/*int i = 0;
	time(&timestart);
	for(; i < 1000000000; i++)
		memcpy((char*)dst1, src1, 2);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));
	int j = 0;
	time(&timestart);
	for(j= 0; j < 1000000000; j++)
		mf_mov16((uint8_t*)dst, (const uint8_t*)src);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));
	int k = 0;
	time(&timestart);
	for(k= 0; k < 1000000000; k++)
		*(uint32_t*)dst2 = *(uint32_t *)src2;
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));*/
	int k = 0;
	time(&timestart);
	for(k= 0; k < 1000000000; k++)
		memcpy((uint8_t*)dst3, (uint8_t*)src_block, 256);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));

	time(&timestart);
	for(k= 0; k < 1000000000; k++)
		mf_memcpy((uint8_t*)dst3, (uint8_t*)src_block, 256);
	time(&timeend);
	printf("timediff:%f\n", difftime(timeend,timestart));
	//printf("dst:%s\n",(char *)dst);
	//printf("dst:%s\n",(char *)dst1);
	//printf("dst:%s\n",(char *)dst2);
	return 0;
}


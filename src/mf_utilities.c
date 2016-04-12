#include "mf_utilities.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

/*==========================
Generate an uint32_t random number
seed is the current system time(usec)
===========================*/

uint32_t generate_random()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	uint32_t r = rand_r((unsigned int *)&(t.tv_usec));
	return r;
}
/*==========================
Copy len byte of data from memory pointed 
by src inversely to memory pointed by dst
e.g. src = "abcd", dst = "dcba"
===========================*/
void inline inverse_memcpy(void* dst, void* src, uint16_t len)
{
	if(len <= 0 || dst == NULL || src == NULL)
		return;
	else
	{
		uint16_t i = 0;
		src = (char*)src + len - 1;
		while(i < len)
		{
			*(char*)dst++ = *(char*)src--;
			i++;
		}
	}
}
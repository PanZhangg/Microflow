#include "../src/mf_wrapper.h"
#include <stdio.h>

int main()
{
	char * src = "abcd";
	char dst[5];
	dst[4] = '\0';
	set_CPU_instruction();
	mf_mov16((uint8_t*)dst, (const uint8_t*)src);
	printf("dst:%s\n",(char *)dst);
	return 0;
}


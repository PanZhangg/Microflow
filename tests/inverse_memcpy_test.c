#include "../src/mf_utilities.h"
#include "stdio.h"

uint32_t swap_32(uint32_t a)
{
	uint32_t rst;
	asm("movl %1, %%eax;bswap %%eax;movl %%eax,%0;":"=r"(rst):"r"(a):"%eax");
	return rst;
}
uint16_t swap_16(uint16_t a)
{
	uint16_t rst;
	asm("mov %1, %%ax;xchg %%al, %%ah;mov %%ax, %0;":"=r"(rst):"r"(a):"%ah");
	return rst;
}
uint16_t copy_16(char* ptr)
{
	return *(uint16_t*)ptr;
}

int main()
{
	char * src_buffer = "abcdefghijk";
	char dst_buffer[12];
	inverse_memcpy(dst_buffer, src_buffer+2, 2);
	uint32_t test = 0x12345678;
	uint16_t test16 = 0x1234;
	uint64_t test64 = 0x1234567812345678;
	uint16_t test1 = ntoh_16bit(((char*)&test+1));
	uint64_t test64_1 = ntoh_64bit(((char*)&test64));
	printf("ntoh:%x\n",test1);
	printf("ntoh64:%ld\n",test64_1);
	uint32_t rst = swap_32(test);
	uint16_t rst_16 = swap_16(test16);
	printf("test:%x\n",rst);
	printf("test:%x\n",rst_16);
	return 0;
}

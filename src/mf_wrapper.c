#include "mf_wrapper.h"
#include "dbg.h"

/*cpuid function from Intel developer guide*/
static void __cpuid(unsigned int where[4], unsigned int leaf)
{
	asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),"=c"(*(where+2)),"=d"(*(where+3)):"a"
	(leaf));
	return;
}

/*Chech&set the instruction set of CPU*/
void set_CPU_instruction()
{
	unsigned int cpuid_results[4];
	__cpuid(cpuid_results,7);

	if(cpuid_results[1] &(1 << 16))
	{
#define RTE_MACHINE_CPUFLAG_AVX512F //comes from  DPDK ^^	
		log_info("CPU FLAG:AVX512F");
		return;
	}
	if(cpuid_results[1] & (1 << 5))
	{
#define RTE_MACHINE_CPUFLAG_AVX2
		log_info("CPU FLAG:AVX2");
		return;
	}
	__cpuid(cpuid_results,1);
	if(cpuid_results[3] & (1 << 25))
		log_info("CPU FLAG:SSE");
}

void mf_mov16(uint8_t * dst, const uint8_t * src)
{
	__m128i xmm0;
	xmm0 = _mm_loadu_si128((const __m128i *)src);
	_mm_storeu_si128((__m128i *)dst, xmm0);
}

void mf_mov32(uint8_t * dst, const uint8_t *src)
{
	mf_mov16((uint8_t *)dst + 0 * 16, (const uint8_t *)src + 0 * 16);
	mf_mov16((uint8_t *)dst + 1 * 16, (const uint8_t *)src + 1 * 16);
}
void mf_mov64(uint8_t * dst, const uint8_t *src)
{
	mf_mov16((uint8_t *)dst + 0 * 16, (const uint8_t *)src + 0 * 16);
	mf_mov16((uint8_t *)dst + 1 * 16, (const uint8_t *)src + 1 * 16);
	mf_mov16((uint8_t *)dst + 2 * 16, (const uint8_t *)src + 2 * 16);
	mf_mov16((uint8_t *)dst + 3 * 16, (const uint8_t *)src + 3 * 16);
}

inline void mf_memcpy(void *dst, void *src, unsigned int n) 
{
	uint8_t * pdst = (uint8_t*)dst;
	uint8_t * psrc = (uint8_t*)src;
	while(n >= 8)
	{
		*(uint64_t *)pdst = *(uint64_t *)psrc;
		pdst += 8;
		psrc += 8;
		n -= 8;
	}
	if(n > 4)
	{
		*(uint32_t *)pdst = *(uint32_t *)psrc;
		pdst += 4;
		psrc += 4;
		n -= 4;
	}
	if(n > 2)
	{
		*(uint16_t *)pdst = *(uint16_t *)psrc;
		pdst += 2;
		psrc += 2;
		n -= 2;
	}
	if(n > 1)
	{
		*(uint16_t *)pdst = *(uint16_t *)psrc;
		//pdst += 2;
		//psrc += 2;
		//n -= 2;
	}
}


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
#define RTE_MACHINE_CPUFLAG_AVX512F //come from  DPDK ^^	
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

//inline void mf_memcpy(void *dst, void *src, unsigned int n) __attribute__((always_inline));

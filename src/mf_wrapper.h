#ifndef MF_WRAPPER_H__
#define MF_WRAPPER_H__
#include <stdlib.h>
#include <emmintrin.h>
void set_CPU_instruction();
inline void mf_memcpy(void *dst, const void *src, unsigned int n) __attribute__((always_inline));
void mf_mov16(uint8_t * dst, const uint8_t * src);

#endif

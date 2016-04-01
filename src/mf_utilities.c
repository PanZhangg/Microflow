#include "mf_utilities.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t generate_random()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	uint32_t r = rand_r((unsigned int *)&(t.tv_usec));
	return r;
}

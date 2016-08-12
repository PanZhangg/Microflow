/*
* Copyright (c) 2009 windyrobin
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* $Id: wrmpool.c 3/18/2009 windyrobin@Gmail.com nanjing china$
*/
#include <stdio.h>
#include <stdlib.h>
#include "wrmpool.h"

void wr_mpool_init(wr_mpool *pmp ,char *begin ,size_t len)
{
	pmp->begin=begin;
	pmp->len = len;
	pmp->index = 0;
	pmp->cflag = 0;
}

void *wr_mpool_malloc(wr_mpool *pmp ,size_t mlen)
{
	void *ret = NULL;
	int rIndex = pmp->index + mlen;
	if(rIndex > pmp->len){
		ret = malloc(mlen);
		pmp->cflag = 1;
	}else{
		ret = pmp->begin + pmp->index;
		pmp->index = rIndex;
	}
	return ret;
}

void wr_mpool_free(wr_mpool *pmp ,void *p)
{	
	/*only free when is allocated in heap*/
	if(p < pmp->begin || p>=pmp->begin + pmp->len){
		free(p);
	}
}

void wr_mpool_clear(wr_mpool *pmp)
{
	pmp->index = 0;
	pmp->cflag = 0;
}

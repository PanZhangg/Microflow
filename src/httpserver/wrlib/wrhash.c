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
 * $Id: wrhash.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "wrhash.h"

void wr_hash_init(wr_hashmap* hm ,wr_mpool *mp ,size_t _size)
{
	assert(mp != NULL);
	hm->mpool = mp;
	hm->buckets = (wr_hashlist **)wr_mpool_malloc(mp ,sizeof(int) * _size);
	//hm->buckets = (wr_hashlist **)malloc(sizeof(int) * _size);
	memset(hm->buckets ,0 ,sizeof(int) * _size);
	hm->size = _size;

	/*in default we use string fun
	so you must init hash before you set hashcmp 
	and hashfun ,otherwise it would be overwrite*/
	hm->hashcmp = wr_hashcmp_str;
	hm->hashfun = wr_hashfun_str;
}

void wr_hash_clear(wr_hashmap *hm)
{
	wr_hashlist *nl,*cl;
	int i;
	
	do{
		/*needn't to clear*/
		if(hm->mpool->cflag == 0)
			break;
		for(i=0 ;i<hm->size ;++i){
			cl = hm->buckets[i];
			while(cl != NULL){
				nl = cl->next;
				wr_mpool_free(hm->mpool ,cl);
				//free(cl);
				cl = nl;
			}		
		}
		//free(hm->buckets);
		wr_mpool_free(hm->mpool ,hm->buckets);
	} while(0);

	memset(hm ,0 ,sizeof(*hm));
}

int wr_hash_add(wr_hashmap *hm ,const void *key ,const void *value)
{
	int pos = hm->hashfun(key) % hm->size;
	wr_hashlist * nhl = (wr_hashlist *)wr_mpool_malloc(hm->mpool ,sizeof(wr_hashlist));
	//wr_hashlist * nhl = (wr_hashlist *)malloc(sizeof(wr_hashlist));
	//if(hm->buckets[pos] == NULL)
	//	++hm->used;

	nhl->key = key,nhl->value=value;
	nhl->next= hm->buckets[pos];
	hm->buckets[pos] = nhl;

	return pos;
	//++(hm->length);
	//printf(" %d" ,pos);
	//if(hm->length%8 == 0) printf("\n");
}

void* wr_hash_find(const wr_hashmap *hm ,const void *key)
{
	int pos = hm->hashfun(key) % hm->size;
	wr_hashlist *nlh = hm->buckets[pos];
	void *ret = NULL;
	while(nlh != NULL){
		if(hm->hashcmp(nlh->key ,key)){
			ret = nlh->value;
			break;
		}
		nlh = nlh->next;
	}
	return ret;
}

/*
37-300 ,132 are ok 
37-1025 852 are ok
all is 37 ,used  >= 34*/
int wr_hashfun_str(const char *s)
{
	int even ,odd;
	int i = 0;
	int mask = 0x1F;
	int ret;
	even = odd = 0;
	while(*s != '\0'){
		if(i&1) odd ^= *s;
		else even ^= *s;
		++s;
		++i;
	}
	//only last 5bit are consider
	ret = even&mask;
	ret <<= 5;
	ret += (odd&mask);
	return ret;
}
/*
37-300 ,110 are ok 
37-1025 822 are ok
all is 37 ,used  >= 34*/
int wr_hashfun_prime(const char *s)
{
	/*2 ,3 ,5 ,7 ,11 ,13 ,17,19,23,29,31*/
	int primeflag[32]={
		0 ,0 ,1 ,1 ,0 ,1 ,0 ,1 ,
		0 ,0 ,0 ,1 ,0 ,1 ,0 ,0 ,
		0 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,
		0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 
	};
	int mask = 0x1F;
	int i=0;
	int pValue = 0;
	int nValue=0;
	int ret;
	while(*s){
		if(i<32 && primeflag[i]) pValue ^= *s;
		else nValue ^= *s;
		++s;
		++i;
	}
	ret = pValue&mask;
	ret <<= 5;
	ret += nValue&mask;

	return ret;
}
/*
37-300 ,127 are ok 
37-1025 837 are ok
all is 37 ,used  >= 34*/
int wr_hashfun_elf(const char *s)
{
	unsigned long h,g;
	h=0;
	while(*s){
		h = (h<<4) + *s++;
		g = h & 0xf0000000;
		if(g) h ^= g>>24;
		h &=  ~g;
	}
	return h;
}


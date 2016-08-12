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
 * $Id: analysis.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */

#include "analysis.h"
#include "misc.h"

/*check is supported ,in misc.h*/
int checkmethod(WrHttp *pHttp)
{
	char *pBuf = pHttp->curPos;
	int ret =-1;
	int i;

	pHttp->method = pBuf;
	SKIP(&pBuf ,' ');

	pHttp->curPos = pBuf;
	for(i=0 ;i<3 ;++i){
		if(0 == strcmp(methods[i] ,pHttp->method)){
			ret = i;	
			break;
		}
	}
	return ret;
}

int parseURL(WrHttp *pHttp)
{
	/*we should treat url carefully to prevent security problems 
	for we will decode url and do something	according the url*/
	char *ePos = NULL;
	char *pBuf = pHttp->curPos;
	TRIM_HEAD(&pBuf);
	pHttp->url = pBuf;
	SKIP(&pBuf ,' ');
	TRIMI_LB_TAIL(pBuf);

	TRIM_HEAD(&pBuf);
	pHttp->protocol = pBuf;
	SKIP(&pBuf ,'\n');
	TRIMI_LB_TAIL(pBuf);

	pHttp->curPos = pBuf;
	pHttp->queryString = NULL;
	if(*(pHttp->method) == 'G'){
		/*if empty ,index file*/
		if(*(pHttp->url) == '\0'){
			pHttp->url = (char *)IndexFile;
		}else if((ePos=strchr(pHttp->url ,'?')) != NULL){
			*ePos = '\0';
			pHttp->queryString = ++ePos;
			pHttp->paramEndPos = find_lb_end(pHttp->protocol);
		}
	}
	/*convert it to utf-8*/
	return url_decode(pHttp->url ,pHttp->url,0);
}

int parseHeader(WrHttp *pHttp)
{	
	char *pBuf = pHttp->curPos;
	int i = -1;
	char *key ,*val;

	/*set bucket size 8 would be efficient in most cases
	it's easy for clean ,and hashfun is just the first letter!*/
	wr_hash_init(&(pHttp->headMap) ,&(pHttp->mp) ,8);
	pHttp->headMap.hashfun = wr_hashfun_uchar;
	pHttp->headMap.hashcmp = wr_hashcasecmp_str;
	
	//while(*pBuf !='\n' && *pBuf!='\t'){
	//if control character ,stop ,(\t \n \0)
	while(*pBuf >= 0x20 && ++i<MAX_HTTP_HEADERS){
		key = pBuf;
		SKIP(&pBuf ,':');
		TRIMI_LB_TAIL(pBuf);

		TRIM_HEAD(&pBuf);
		val = pBuf;
		SKIP(&pBuf ,'\n');
		TRIMI_LB_TAIL(pBuf);

		wr_hash_add(&(pHttp->headMap) ,key ,val);
	}
	/*if heads num exceed or SKIP run flash ,return error*/
	if(++i <= MAX_HTTP_HEADERS && pBuf - pHttp->method <pHttp->recvLen){
		/*skip the blank line ,if POST is used,we now reach postdata-head*/
		SKIP(&pBuf ,'\n');
		pHttp->curPos = pBuf;
	}else{
		i =  -1;
	}
	return i;
}

const char *get_head_info(const WrHttp *pHttp ,const char *key)
{
	return wr_hash_find(&(pHttp->headMap) ,key);
}

const char *get_param_info(const WrHttp *pHttp ,const char *key)
{
	return wr_hash_find(&(pHttp->paramMap) ,key);
}

void print_header(const WrHttp *pHttp)
{
	int i;
	wr_hashlist *cList ;

	printf("\n%s %s %s\n" ,pHttp->method ,pHttp->url ,pHttp->protocol);
	for(i=0 ;i<pHttp->headMap.size ;++i){
		cList = pHttp->headMap.buckets[i];
		while(cList != NULL){
			printf("%s:%s\n" ,cList->key ,cList->value);
			cList = cList->next;
		}
	}
}

void print_param(const WrHttp *pHttp)
{
	int i;
	wr_hashlist *cList ;
	for(i=0 ;i<pHttp->paramMap.size ;++i){
		cList = pHttp->paramMap.buckets[i];
		while(cList != NULL){
			printf("%s:%s\n" ,cList->key ,cList->value);
			cList = cList->next;
		}
	}
}

/*
check and fix the path , if path begin with '/' ,skip the seprator;
if ok return 0 :static file ,1:cgi ,
else return 404 or 400;

!!note ,the path GET/POST by utf8-encoded ,
so you must convert it to ANSI before you call function like
stat ,read,write....
*/
int checkpath(WrHttp *pHttp)
{
	int ret = 404;
	char *ext = NULL;
	/*sql injection!!!*/
	do 
	{
		
		if(strstr(pHttp->url ,"..") != NULL){
			DBG("may be sql injection..");
			ret = 400;
			break;
		}
		DBG("utf8 : len,%d s: %s\n" ,strlen(pHttp->url) ,pHttp->url);
		UTF8toANSI(pHttp->url);
		DBG("ansi : len,%d s: %s\n" ,strlen(pHttp->url) ,pHttp->url);

		if(*(pHttp->url) == '/')
			++(pHttp->url);
		if(*(pHttp->url) == '\0')
			pHttp->url = (char *)IndexFile;

		if(stat(pHttp->url ,&pHttp->st) == 0)
			ret = 0;
		else{
			ext=strchr(pHttp->url ,'.');
			if(ext!=NULL && strcmp(ext ,CgiExt) == 0){
				/*we only use string befor .cgi to decide cgipage*/
				ret = 1;
			}
		}
	} while(0);

	return ret;
}

/*only check cookie when cgi process,
for it's non-meaning when client get static file*/
int cookieCheck(const WrHttp *pHttp)
{
	return 0;
}

/*return 200 ,or 304*/
int cacheCheck(const WrHttp *pHttp)
{
	int retCode = 200;
	char buf[64];
	const char *oldETag = get_head_info(pHttp ,"If-None-Match");
	do{
		if(oldETag == NULL) 
			break;
		sprintf(buf ,"%lx.%lx" ,pHttp->st.st_size ,pHttp->st.st_mtime);
		if(strcmp(oldETag ,buf) == 0) retCode = 304;
	} while(0);
	return retCode;	
}

int decodeParam(WrHttp *pHttp)
{
	static const char *skipBrake = "&=&=";
	char *pBuf ,*endPos;
	char backup[8];
	char *key ,*val;
	int ret = 0;

	/*of course ,you can use postData ,they are union*/
	DBG("params: \n%s\n" ,pHttp->postData);
	pBuf = pHttp->queryString;
	endPos = pHttp->paramEndPos;

	/*buckets 32 would be ok ,and we use the default strcmp and hashfun */
	wr_hash_init(&(pHttp->paramMap) ,&(pHttp->mp) ,32);
	
	/*must place after init */
	if(pBuf==NULL || *pBuf == '\0') return 0;

	memcpy(backup ,endPos ,4);
	memcpy(endPos ,skipBrake ,4);
	do{
		key=pBuf;
		SKIP(&pBuf ,'=');
		val=pBuf;
		SKIP(&pBuf ,'&');
		if(url_decode(key ,key ,1)<0 || url_decode(val ,val ,1)<0){
			ret = -1;
			break;
		}
		/*the key should be null ,but val could be*/
	} while(*key!='\0' && wr_hash_add(&(pHttp->paramMap) ,key ,val)>=0 && pBuf < endPos);
	/*if GET method ,we would override so we must restore the data ,*/
	memcpy(endPos ,backup ,4);

	return (ret ==0 && pBuf== ++endPos) ? 1 : -1 ;
}


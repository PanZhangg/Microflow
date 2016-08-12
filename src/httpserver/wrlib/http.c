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
 * $Id: http.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */
#include "../cgi.h"
#include "misc.h"
#include "http.h"
#include "wrhash.h"

/*
void wr_lock()
{
	#ifdef _WIN32
	EnterCriticalSection(&WrContext.lock);
	#else
	pthread_mutex_lock(&WrContext.lock);
	#endif
}

void wr_unlock()
{
	#ifdef _WIN32
	LeaveCriticalSection(&WrContext.lock);
	#else
	pthread_mutex_unlock(&WrContext.lock);
	#endif
}
*/

void wr_sleep(size_t ms)
{
	#ifdef _WIN32
	Sleep(ms);
	#else
	usleep(ms*1000);
	#endif
}

int wr_uninit()
{
	WrContext.quitFlag = 1;
	while(WrContext.threadCnt >0) wr_sleep(WR_SOCK_RTT);
	/*may be you have some memory to be freed*/
	cgi_uninit();

	/*
	#ifdef _WIN32
	DeleteCriticalSection(&WrContext.lock);
	#else
	pthread_mutex_destroy(&WrContext.lock);
	#endif
	*/
	wr_hash_clear(&WrContext.mimeMap);
	wr_hash_clear(&WrContext.pageMap);
	printf("...robin quit...\n");
	return 0;
}


/*defined in request.c*/
void  requestHandler(void * s);

static int wr_http_start()
{
	SOCKET ser_fd, cli_fd;  /* listen on sock_fd, new connection on new_fd */
	struct sockaddr_in ser_addr ,cli_addr; /* connector's address information */
	int opt ,sin_size;
	/*
	* Setup the default values
	*/
	#ifdef _WIN32	
	int tv = WR_SOCK_RTT;
	WSADATA ws;
	WSAStartup(0x202,&ws);
	#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = WR_SOCK_RTT*1000;
	#endif
	/*
	* Setup the sockets and wait and process connections
	*/
	
	if ((ser_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	/* Let the kernel reuse the socket address. This lets us run
	twice in a row, without waiting for the (ip, port) tuple
	to time out. */
	opt = 1;
	setsockopt(ser_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	setsockopt(ser_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
	
	ser_addr.sin_family = AF_INET;         /* host byte order */
	ser_addr.sin_port = htons(PORT);     /* short, network byte order */
	ser_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
	
	if (bind(ser_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) == -1){
		perror("bind");
		exit(1);
	}
	
	if (listen(ser_fd, WR_SOCK_BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	chdir(RootPath);
	DBG("\nwrHttp is running...\n");
	sin_size = sizeof(struct sockaddr_in);
	while(1){  /* main accept() loop */
		if(WrContext.quitFlag == 1)
			break;
		if ((cli_fd = accept(ser_fd, (struct sockaddr *)&cli_addr, &sin_size)) == -1)
			continue;
		while(1){
			if(WrContext.threadCnt < WR_MAX_THREADS){
				start_thread(requestHandler ,cli_fd);
				break;
			}
			else
				wr_sleep(50);
		}
	}
	return 0;
}

const char *get_mime_type(const char *path)
{
	const char	*extension;
	const char *ret=NULL;
	if (path!=NULL && (extension = strrchr(path, '.')) != NULL) {
		ret = wr_hash_find(&WrContext.mimeMap ,++extension);
	}
	return (ret!=NULL) ? ret : ("text/plain");
}

void cgi_page_add(const char * pageName,\
				  void *f)
{
	wr_hash_add(&WrContext.pageMap ,pageName ,f);
}

void* cgi_page_find(const char *pageName)
{
	return wr_hash_find(&WrContext.pageMap ,pageName);
}

int wr_init()
{
	int i=-1;
	/*(97+97) * 4 = 800 ,assume page number is 50 and mimitype is 50 ,
	so 100*sizeof(hash_list) = 1200 ,so 2048 is just ok*/
	static char pool[2048];
	static wr_mpool mpool;

	wr_mpool_init(&mpool ,pool ,sizeof(pool));
	wr_hash_init(&WrContext.mimeMap ,&mpool ,97);
	wr_hash_init(&WrContext.pageMap ,&mpool ,97);
	WrContext.pageMap.hashcmp = wr_hashcasecmp_str;
	WrContext.mimeMap.hashcmp = wr_hashcasecmp_str;
	/*add mime type map*/
	while(mmt[++i].ext != NULL){
		wr_hash_add(&WrContext.mimeMap ,mmt[i].ext ,mmt[i].type);
	}
	
	WrContext.quitFlag = 0;
	WrContext.threadCnt = 0;
	
	/*
	#ifdef _WIN32
	InitializeCriticalSection(&WrContext.lock);
	#else
	pthread_mutet_init(&WrContext.lock ,NULL);
	#endif
	*/
	/*you must realize this fun in cgi.c*/
	cgi_init();
	return start_thread(wr_http_start ,NULL);
}


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
 * $Id: wrio.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */
#include "wrio.h"
#include "misc.h"
#include "analysis.h"
#include "wrstring.h"

static int sendFileStreamPOSIX(const WrHttp *pHttp ,const char *filePath)
{
	char buf[PAGE_SIZE];
	int rSize;
	int ret;
	/*must add O_BINARY on windows ,it cause me dubug for 2hours ,fuck!!*/
	int fd = open(filePath ,O_RDONLY|O_BINARY);
	assert(fd >=0);
	while((rSize =read(fd ,buf ,sizeof(buf))) > 0){
		if((ret=wr_sock_nwrite(pHttp->sock ,buf ,rSize)) <0)
			break;
		//printf("rSize ,ret : %d, %d\n" ,rSize ,ret);
	}
	close(fd);
	//printf("WSAERRPR :%d\n" ,WSAGetLastError());
	return ret;
}

#ifdef _WIN32
static int sendFileStreamWin32(const WrHttp *pHttp ,const char *filePath)
{
	HANDLE hFile;  
	DWORD  dwBytesRead; 
	char   buf[PAGE_SIZE]; 
	int ret;

	hFile = CreateFile(filePath,     // open ONE.TXT 
		GENERIC_READ,                 // open for reading 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_EXISTING,                // existing file only 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE){ 
		DBG("Could not open %s" ,filePath);  // process error 
		return -1;
	} 
	do{ 
		if (ReadFile(hFile, buf, sizeof(buf), &dwBytesRead, NULL)) { 
			if((ret=wr_sock_nwrite(pHttp->sock ,buf ,dwBytesRead)) < 0)
				break;
		} 
	} while (dwBytesRead == PAGE_SIZE); 
	CloseHandle(hFile);
	return ret;
}
#endif

/*send file stream ,if ok return the final send size ,else 
return last send error ,
maybe you want to rewrite the code use filemapping/mmap or 
win32 api ,here i just call POSIX interface! 
*/
int sendFileStream(const WrHttp *pHttp ,const char *filePath)
{
#ifdef _WIN32
	return sendFileStreamWin32(pHttp ,filePath);
#else
	return sendFileStreamPOSIX(pHttp ,filePath);
#endif
}

/*if EINTR ,try once*/
static int wr_sock_recv(SOCKET sock ,char *buf ,size_t bufsize)
{
	int ret;
	do {
		ret = recv(sock ,buf ,bufsize ,0);
	}while(ret<0 && WR_SOCK_ERRNO == EINTR);
	return ret;
}

/*read exactly n bytes ,
if ok return n
else retrun -1
here we use macro recv-timeout defined in http.h*/

int wr_sock_nread(SOCKET sock ,char *buf, size_t bufsize ,size_t n)
{
	size_t nRead = 0;
	size_t nLeft = n;
	int ret;
	assert(bufsize >= n);
	while(nLeft > 0){
		ret =wr_sock_recv(sock ,buf+nRead ,MIN(nLeft,WR_TCP_QUAN));
		/*socket closed or error occurred*/
		if(ret <= 0)
			break;
		else{
			nRead += ret;
			nLeft -= ret;
		}
	}
	return nRead==n ? n : -1;
}

/*if ok return n ,else
return -1*/
int wr_sock_nwrite(SOCKET sock ,char *buf ,size_t n)
{
	size_t nLeft = n;
	size_t nWrite =0;
	int ret;
	while(nLeft > 0){
		ret =send(sock ,buf+nWrite ,nLeft ,0);
		/*socket closed or error occurred*/
		if(ret==0 || (ret<0 && WR_SOCK_ERRNO != EINTR))
			break;
		else{
			nLeft -= ret;
			nWrite += ret;
		}
	}
	return nWrite==n ? n : -1;
}

/*
\r\n\r\n or \n\n ,they can be separated in any position!!
@pBuf the data buf
@cPos current-check begin pos
@len  len to be checked(from cPos)
if OK return i>=0
else return -1
*/
static int isHeadEnd(char *pBuf ,char *cPos ,int len)
{
	int i=-1;
	while(++i < len){
		if('\n' == *cPos){
			if(	(cPos-1>=pBuf && '\n'==*(cPos-1)) || \
				(cPos-2>=pBuf && '\r'==*(cPos-1) && '\n'==*(cPos-2)) ){
				return i;
			}
		}
		++cPos;
	}
	return -1;
}

/*if read head ok return nread
if some error occurred ,return -1*/
int wr_read_head(SOCKET sock ,char *buf ,size_t bufsize)
{
	int nRead = 0;
	int rsize;
	int flag = -1;
	do {
		rsize = wr_sock_recv(sock ,buf+nRead ,bufsize-nRead);
		if(rsize <=0)
			break;
		flag = isHeadEnd(buf ,buf+nRead ,rsize);
		nRead += rsize;
	}while(flag < 0);

	return (flag <0) ? -1 : nRead;
}

/*if error occurred ,return -1,
and send http-400 ,bad request,
*/
int wr_load_body(WrHttp *pHttp)
{
	/*!!note ,we shouldn't decode querystring or post data here
	for they may contain character like '&', '=', or something like that*/
	int ret;
	int cLen ,rLen ,sLen;
	const char *pType ,*pLen;
	do{
		/*"x-www-form-urlencoded"*/
		pType = get_head_info(pHttp ,"Content-Type");
		pLen =  get_head_info(pHttp ,"Content-Length");

		if(pType == NULL || pLen == NULL){
			ret = -1;
			break;//error
		}
		/*content len*/
		cLen = atoi(pLen);
		if(cLen<= 0 || cLen >MAX_POST_SIZE){
			ret = -2;
			break;//error
		}
		/*len we required (include head)*/
		rLen = pHttp->curPos - pHttp->method + cLen;
		/*content len we have got*/
		sLen = pHttp->recvLen - (pHttp->curPos-pHttp->method);

		DBG("cLen ,rLen ,sLen: %d %d %d\n" ,cLen ,rLen ,sLen);	
		if(cLen == sLen){
			DBG("just ok..\n");
			pHttp->postData = pHttp->curPos;
			ret = 1;
		}else if(cLen >sLen){
			/*pre recv buf is too short to hold the content we require
			so we should read socket again
			!!note ,you must check pHttp->postData==pHttp->curPos otherwise
			you should free it*/
			if(cLen + pHttp->curPos - pHttp->method > MAX_HEADER_SIZE){
				pHttp->postData = malloc(cLen+8);
				memmove(pHttp->postData ,pHttp->curPos ,sLen);
			}
			ret = wr_sock_nread(pHttp->sock ,pHttp->postData + sLen ,cLen-sLen ,cLen-sLen);
			if(cLen-sLen == ret){
				ret = 2;
			}else
				ret = -3;
		}else{
			ret = -4;
		}
	}while(0);
	if(ret >0){
		pHttp->paramEndPos = pHttp->postData + cLen;
		*(pHttp->paramEndPos) = '\0';
	}
	DBG("contentType : %d\n" ,ret);
	return ret;
}


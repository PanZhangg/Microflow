
#include "wrstring.h"
#include "misc.h"
#include "analysis.h"
#include "wrio.h"
#include "../cgi.h"

/*set mimetype  ,content length ,and cache control 
  like expire and last-modified
*/
static int fileSet(char *buf ,const WrHttp *pHttp)
{
	//	strftime(timeStr ,sizeof(timeStr) ,st.st_mtime)
	//printf("%llu\n" ,st.st_size);
	return sprintf(buf ,"Content-Type: %s\n"
				"Content-Length: %d\n"
				"Cache-Control: max-age=%ld\n"
				"ETag: %lx.%lx\n",
				get_mime_type(pHttp->url),
				pHttp->st.st_size,
				ExpireTime,
				pHttp->st.st_size,
				pHttp->st.st_mtime
				);
}

static int typeSet(char *buf ,const char *type)
{
	return sprintf(buf ,"Content-Type: %s\n" ,get_mime_type(type));
}

static int lengthSet(char *buf ,int len)
{
	return sprintf(buf ,"Content-Length: %d\n" ,len);
}

/*session and cookie detect ,if you need it
please redifine it*/
static int cookieSet(char *buf)
{
	return 0;
}

/*if keep alive was used !
  should check httpinfo like "Connection:close"
  but here ,i just set it always live when send file!!
  if flat is '1' ,will always set 'close',this mean some error has occurred 
  httpInfo was ingnored 
  */
static int liveSet(char *buf ,const WrHttp *pHttp ,int closeflag)
{
	return sprintf(buf ,"Connection: %s\n" ,(closeflag==1) ? "close" :"Keep-Alive")	;
	//return sprintf(buf ,"Connection: close\n\n")	;
}

/* just add '\n' to head info ,only here we use 'send' method 
in order to be portable in future*/
static int sendHead(const WrHttp *pHttp ,char *pBuf ,size_t len)
{
	size_t nLen;
	nLen = sprintf(pBuf+len ,"Server :"SERVER"\n\n");
	DBG("%s" ,pBuf);
	return wr_sock_nwrite(pHttp->sock ,pBuf ,len+nLen);
}

static int codeSet(char *pBuf ,int code)
{
	static const char *c200 = "OK";
	static const char *c304 = "Not Modified";
	static const char *c400 = "Bad Request";
	static const char *c404 = "Not Found";
	static const char *c501 = "Not Implemented" ;

	const char *msg =NULL;
	switch(code){
		case 200:
			msg= c200;
			break;
		case 304:
			msg = c304;
			break;
		case 400:
			msg = c400;
			break;
		case 404:
			msg = c404;
			break;
		case 501:
			msg = c501;
			break;
		default:
			break;
	}
	return sprintf(pBuf ,"HTTP/1.1 %d %s\n" ,code ,msg);
}

/*
now only support:
400 Bad Request
404 not Found
405 Method not Allow
if error occured ,the IE/FF will close the socket,
so we should alse break the seeion thread

NOTE: 304 Not Modified and 200 OK shouldn't be passed here
they are default keep-alive!!
*/
int wr_error_reply(const WrHttp *pHttp ,int stscode)
{
	/*only error code and server info are send  ,128 would be ok!
	and server string shouldn't too long*/
	char buf[128];
	char *pBuf = buf;
	int closeflag = 1;
	
	pBuf += codeSet(pBuf ,stscode);
	//pBuf += liveSet(pBuf ,pHttp ,1);
	/*if method is head or get ,this mean only head info ,body not exsited,
	we could reuse the thread*/
	/*if(*(pHttp->method) <= 'H')
		if(pHttp->curPos - pHttp->method < WR_BUF_SIZE)
			closeflag = 0;
	pBuf += liveSet(pBuf ,pHttp ,closeflag);
	printf("%s\n" ,pHttp->filePath);
	ret =sendHead(sock ,buf ,pBuf);
	if(ret >= 0 && 0==closeflag) ret = 0;
	else ret = -1;
	*/
	return sendHead(pHttp ,buf ,pBuf-buf);
}

int wr_send_msg(WrHttp *pHttp ,const char *type ,const char *buf ,size_t len)
{
	char hBuf[BUFSIZ];
	char *pBuf = hBuf;
	int ret;
	pBuf += codeSet(pBuf ,200);
	pBuf += typeSet(pBuf ,type);
	pBuf += lengthSet(pBuf ,len);

	do{
		if((ret=sendHead(pHttp ,hBuf ,pBuf-hBuf)) <0)
			break;
		if(ret=wr_sock_nwrite(pHttp->sock ,buf ,len) <0)
			break;
	} while(0);
	return ret;
}

/*this function should only be called by cgi process 
so pHttp->stat and pHttp->filePath is not used 
here we reuse the struct
if ok return 0 or last send size
else reutrn < 0
*/
int wr_send_file(WrHttp *pHttp ,const char *filePath)
{
	char buf[BUFSIZ];
	char *pBuf = buf;
	int ret = 0;
	pBuf += codeSet(pBuf ,200);
	pHttp->url = (char *)filePath;
	stat(filePath ,&pHttp->st);
	pBuf += fileSet(pBuf ,pHttp);
	do 
	{
		if((ret =sendHead(pHttp ,buf ,pBuf-buf)) <0)
			break;
		if((ret =sendFileStream(pHttp ,filePath)) <0)
			break;
	} while(0);
	return ret;
}

static int staticProcess(const WrHttp *pHttp)
{
	char buf[BUFSIZ];
	char *pBuf = buf;
	int ret =0;

	int code = cacheCheck(pHttp);
	pBuf += codeSet(pBuf ,code);
	/*default is keep-alive*/
	//pBuf += liveSet(pBuf ,pHttp ,0);
	if(code == 200){
		pBuf += fileSet(pBuf ,pHttp);
	}
	do{
		/*if send error return -1*/
		if((ret=sendHead(pHttp,buf ,pBuf-buf)) < 0)
			break;
		/*if 304 or method == HEAD return 0k*/
		if(code==304 || 'H'==*(pHttp->method))
			break;
		ret = sendFileStream(pHttp ,pHttp->url);
	} while(0);
	return ret;
}

/*if ok return 0
else return retcode*/
static int cgiProcess(WrHttp *pHttp)
{
	int ret = 0;
	void *handle = NULL ;
	do{
		if((handle=cgi_page_find(pHttp->url)) == NULL){
			errorLog(pHttp ,"cgi page not find");
			ret = -1;
			break;
		}
		/*when is POST method ,maybe we should read socket*/
		if(('P'==*(pHttp->method)) && wr_load_body(pHttp) < 0){
			errorLog(pHttp ,"body check error");
			ret = -2;		
			break;
		}
		if(decodeParam(pHttp) < 0){
			errorLog(pHttp ,"param decode error");
			ret = -3;
			break;
		}
		if(cgi_handler(pHttp ,handle) < 0){
			errorLog(pHttp ,"handler error");
			ret = -4;
		}
	}while(0);

	/*when postData is two long ,we malloc postData ,
	so we must free it*/
	if(*(pHttp->method)=='P' && pHttp->postData!=pHttp->curPos) free(pHttp->postData);

	wr_hash_clear(&(pHttp->paramMap));
	return ret;
}

static void clearHttp(WrHttp *pHttp)
{
	wr_hash_clear(&(pHttp->headMap));
	wr_mpool_clear(&(pHttp->mp));
}

/*if some error occured ,break the session
if code <400 ,return 0*/
static int replyHandler(WrHttp *pHttp)
{
	int rType;
	int ret = -1;
	
	rType = checkpath(pHttp);
	DBG("filetype : %d\n" ,rType);
	switch (rType){
		case 0://static file
			ret = staticProcess(pHttp);
			break;
		case 1://dyanamic page,'.cgi' extension
			ret = cgiProcess(pHttp);
			break;
		default:
			wr_error_reply(pHttp ,rType);
	}
	clearHttp(pHttp);
	/*if error occured ,return -1 ,we should break the session*/
	return ret;
}


void  requestHandler(void * s)
{
	SOCKET sock= (SOCKET)s;
	char recvBuf[MAX_HEADER_SIZE + 8];

	/*head buckets is 8 ,param buckets is 32 ,(8+32)*4 = 160 
	left 352 ,352/sizeof(hash_list) = 352/12 = 30,i think that's enough*/
	char pool[512];
	int ret = -1;
	static const char *skipBrake = "  \n\n:\n\n";
	WrHttp httpInfo;
	
	++WrContext.threadCnt;

	httpInfo.sock = sock;
	wr_mpool_init(&httpInfo.mp ,pool ,sizeof(pool));
	/*we break the connetion(or thread ,session) to clear socket
	  when any error occurred!
	 */
	do{
		if(WrContext.quitFlag == 1)
			break;
		httpInfo.recvLen =wr_read_head(sock ,recvBuf ,MAX_HEADER_SIZE);
		if(httpInfo.recvLen <= 0)
			break;

		httpInfo.curPos = recvBuf ;
		recvBuf[httpInfo.recvLen] = '\0';
		/*very import!! to avoid SKIP macro to run flash!!*/
		strcat(recvBuf+httpInfo.recvLen ,skipBrake);
		
		/*if method not Implemented*/
		if(checkmethod(&httpInfo) < 0){
			DBG("len :%d %s\n" ,httpInfo.method);
			wr_error_reply(&httpInfo ,501);
			break;
		}
		if(parseURL(&httpInfo) <0){
			wr_error_reply(&httpInfo ,400);
			errorLog(&httpInfo ,"parseURL error");
			break;
		}
		/*if parse head error*/
		if(parseHeader(&httpInfo) < 0){
			wr_error_reply(&httpInfo ,400);//bad Request,should break;to clean data;
			errorLog(&httpInfo ,"parse head error");
			clearHttp(&httpInfo);
			break;
		}
		//print_header(&httpInfo);
		/*if reply error */
		if(replyHandler(&httpInfo) < 0){
			//errorReply(&httpInfo ,400);//bad Request,should break;to clean data;
			//errorLog(&httpInfo ,"reply error");
			break;
		}
	}while(1);

	closesocket(sock);
	--WrContext.threadCnt;

	DBG("---threads : %d----\n" ,WrContext.threadCnt);
}

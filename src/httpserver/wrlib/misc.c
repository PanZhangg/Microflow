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
 * $Id: misc.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */
#include "misc.h"
void DBG(const char *fmt, ...)
{
#ifdef _DEBUG
	va_list	ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
#endif
}
/*
decode src until '\0' is found, 
note that when in "%\0" or "%f\0" cases;
l or h would be 0, while break,and return -1
so the function would't run flash

the result dst will end with '\0'
i didn't check dst size
so you must make sure the dst space is ok
before you call this function
if ok return result length ,else
return -1*/

int url_decode(const char *src ,char *dst ,int is_form_url_encoded)
{
	/*
	encodeURIComponent  table (71):
	!£¬ '£¬(£¬)£¬*£¬-£¬.£¬_£¬~£¬0-9£¬a-z£¬A-Z
	
	encodeURI plus are(11) :
	$£¬&£¬#£¬+£¬,£¬/£¬:£¬;£¬=£¬?£¬@
	
	*/
	static char flag[128]={
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,-1,-1,-1,-1,-1,-1,
		-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	};
	char h,l;
	const char *cp =src;
	char *oldDst = dst;
	int ret = 0;
	//DBG("before decode :\n%s\n" ,src);
	while(*cp != '\0'){
		if(*cp == '%'){
			h=*(++cp);
			l=*(++cp);
			if(h>0 && l>0 && flag[h]>=0 && flag[l]>=0){
					*dst = (flag[h]<<4 | flag[l]);
			}else{
				ret = -1;
				break;
			}
		}else if(*cp == '+' && 1==is_form_url_encoded){
			*dst = ' ';
		}else{
			*dst=*cp;
		}
		++cp;
		++dst;
	}
	*dst = '\0';
	return ('\0'==*cp && ret==0) ? (dst - oldDst) : -1;
}

/*will replace src with new string
return result len
you must assure that the srcLen is larger
than result len!! so UTF8 2 ANSI to ok ,but in reverse
would cause error!!*/

#ifdef _WIN32
static int iconv(char *src, int sourceCodePage,int targetCodePage)
{
	int sLen = strlen(src);
	int uLen = MultiByteToWideChar(sourceCodePage,0,src,-1,NULL,0);   
	//cout<<"unicodeLen :"<<unicodeLen<<endl;
	wchar_t   *   pUnicode; 
	char *pTData   =  NULL; 
	//int   tLen;
	
	pUnicode=(wchar_t *)malloc(sizeof(wchar_t) * (uLen+1));   
	memset(pUnicode,0,(uLen+1)*sizeof(wchar_t));   
	MultiByteToWideChar(sourceCodePage,0,src,-1,pUnicode,uLen);   

	//tLen=WideCharToMultiByte(targetCodePage,0,pUnicode,-1,pTData,0,NULL,NULL);   
	//pTData=(char *)malloc(tLen+1);   
	//memset(pTData,0,tLen+1); 
	//WideCharToMultiByte(targetCodePage,0,pUnicode,-1,pTData,tLen,NULL,NULL);   
	WideCharToMultiByte(targetCodePage,0,pUnicode,-1,src,sLen,NULL,NULL); 
	//printf("sLen ,uLen ,tLen :%d %d\n" ,sLen ,uLen );
	//printf("src ,unicode:\n%s\n%s\n" ,src ,pUnicode);
	free(pUnicode);   
	//free(pTData);   
	return   0; 
}
#endif

int UTF8toANSI(char *src)
{
	#ifdef _WIN32
	return	iconv(src ,CP_UTF8 ,CP_ACP);
	#endif

	/*in linux ,it's utf-8 in default ,
	so it's no need to convert */
	return 0;
	/*
	size_t sLen;
	int ret;
	iconv_t cd;
	sLen = strlen(src);
	cd = iconv_open("" ,"utf-8");
	if(cd ==0) return -1;
	ret  =iconv(cd ,&src ,&sLen ,&src,&sLen);
	iconv_close(cd);
	return ret;
	#endif
	*/
}

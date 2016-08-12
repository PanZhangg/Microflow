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
 * $Id: wrstring.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */

#include <string.h>
#include <ctype.h>

/* add '\0' in token pos ,
* change pBuf to next pos 
*/

/*we have define macro SKIP to replace it

void skip(char **ppBuf ,char token)
{
	char *pBuf = *ppBuf;
	while (*pBuf != token){
		++pBuf;
	}
	*pBuf = '\0';
	*ppBuf = ++pBuf;
}
*/

/**/
int wr_strcasecmp(register const char *lStr ,register const char *rStr)
{
	while(*lStr && *rStr){ 
		if(*lStr !=*rStr && tolower(*lStr)!=tolower(*rStr))
			break;
		++lStr;
		++rStr;
	}
	return *lStr - *rStr;
}
/*
int endscmp(const char *src ,const char *substr)
{
	int llen = strlen(src);
	int rlen = strlen(substr);
	if(rlen <llen)
		return -1;
	return strcmp(src+llen-rlen ,substr);
}
*/
/*return new string*/
char* wr_trim(char *pBuf)
{	
	char *pNew = NULL;
	/*left trim*/
	while(*pBuf <=' ' && *pBuf!='\0'){
		++pBuf;
	}
	//printf("before right:%s\n" ,pBuf);
	pNew = pBuf;	
	if(*pBuf !='\0'){
		/*right trim*/
		//printf("strlen :%d\n" ,strlen(pBuf));
		pBuf += strlen(pBuf);
		while(*(--pBuf) <= ' '){
			//printf("%d" ,*pBuf);
			*pBuf = '\0';
		}
	}	
	//printf("after trim:%s\n" ,pBuf);
	return pNew;
}


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
 * $Id: cgi.c 3/14/2009 windyrobin@Gmail.com nanjing china$
 */
#include "cgi.h"

int cgi_page_sum(WrHttp *pHttp)
{
	const char *lAdd ,*rAdd;
	int sum;
	char buf[32];
	printf("\n--add.cgi--\n");
	
	print_param(pHttp);
	lAdd = get_param_info(pHttp ,"lAdd");
	rAdd = get_param_info(pHttp ,"rAdd");
	sum = atoi(lAdd)+atoi(rAdd);

	sprintf(buf ,"%d" ,sum);
	wr_send_msg(pHttp ,NULL ,buf ,strlen(buf));
	return 0;

}

int cgi_page_txt(WrHttp *pHttp)
{
	printf("\n--txt.cgi--\n");
	print_param(pHttp);
	wr_send_file(pHttp ,"hello.txt");
	
	return 0;
}

int cgi_page_login(WrHttp *pHttp)
{
	const char *smsg="login success";
	const char *emsg="login error";
	static const char *user="robin";
	static const char *pwd="hood";

	const char *pRet=emsg;
	
	const char *pUser ,*pPwd;
	printf("\n--login.cgi--\n");
	print_param(pHttp);

	pUser = get_param_info(pHttp ,"user");
	pPwd = get_param_info(pHttp ,"pwd");
	if(strcmp(user ,pUser)==0 && strcmp(pwd ,pPwd)==0){
		pRet = smsg;
	}
	wr_send_msg(pHttp ,NULL ,pRet ,strlen(pRet));

	return 0;
}

void cgi_init()
{
	cgi_page_add("sum.cgi" ,cgi_page_sum);
	cgi_page_add("txt.cgi" ,cgi_page_txt);
	cgi_page_add("login.cgi" ,cgi_page_login);
}

void cgi_uninit()
{

}

int cgi_handler(WrHttp *pHttp ,void *handle)
{
	int ret = -1;
	int (*pf)(WrHttp *) = handle;
 
	return pf(pHttp);
}

int errorLog(WrHttp *pHttp ,const char *mess)
{
	printf("%s\n" ,mess);
	return 0;
}


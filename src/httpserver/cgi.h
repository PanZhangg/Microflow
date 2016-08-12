#ifndef __CGI_H__
#define __CGI_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "wrlib/http.h"


/*!!! functions that you MUST realize*/

/*add your new cgi page */
void	cgi_init();

/*may be you have some memory to free*/
void	cgi_uninit();

/*you can write page in c,and you can also write it in c++ 
so the page-handle may be a function 
or you can set it a point to a Class Object(c++)*/
int		cgi_handler(WrHttp *pHttp ,void *handle);


/*write you own log*/
int		errorLog(WrHttp *pHttp ,const char *mess);


/*page handler declare here*/
int cgi_page_sum(WrHttp *pHttp);
int cgi_page_txt(WrHttp *pHttp);
int cgi_page_login(WrHttp *pHttp);


#ifdef __cplusplus
}
#endif

#endif


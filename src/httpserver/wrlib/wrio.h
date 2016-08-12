
#include "http.h"

#ifndef  __WR_IO_H__
#define  __WR_IO_H__

#ifdef __cplusplus
extern "C"{
#endif

int wr_sock_nwrite(SOCKET sock ,char *buf ,size_t n);

int wr_read_head(SOCKET sock ,char *buf ,size_t bufsize);
int wr_load_body(WrHttp *pHttp);
int sendFileStream(const WrHttp *pHttp ,const char *filePath);

#ifdef __cplusplus
}
#endif

#endif


#ifndef __MF_CONTROLLER_H__
#define __MF_CONTROLLER_H__
#include "mf_socket.h"
#include "mf_topomgr.h"
#include "mf_devicemgr.h"
extern void* handler_connection(void*);
void controller_start(uint32_t * sock);
void controller_exit();
#endif

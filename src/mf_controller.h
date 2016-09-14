#ifndef __MF_CONTROLLER_H__
#define __MF_CONTROLLER_H__
#include "mf_socket.h"
#include "mf_topomgr.h"
#include "mf_devicemgr.h"
//#include "mf_api.h"

#define NET_RECEIVE_WORKER_NUMBER 1

void controller_start();
void controller_exit();

#endif

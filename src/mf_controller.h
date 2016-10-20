#ifndef __MF_CONTROLLER_H__
#define __MF_CONTROLLER_H__
#include "mf_socket.h"
#include "mf_topomgr.h"
#include "mf_devicemgr.h"
//#include "mf_api.h"

#define NET_RECEIVE_WORKER_NUMBER 1
#define MAX_NAME_LENGTH 32
#define IP_ADDR_LENGTH 4

struct mf_controller
{
	char controller_name[MAX_NAME_LENGTH];
	char * configFile;
	char * logFile;
	int port;                   /* TCP listening port */
    	char * bindaddr[IP_ADDR_LENGTH]; /* Addresses we should bind to */
	int sock_fd;	
};

void controller_init(struct mf_controller * controller, char * name);
void controller_start();
void controller_exit();

#endif

#include "mf_controller.h"
#include "mf_socket.h"
#include "mf_logger.h"
#include "dbg.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

uint32_t listen_sockfd;
extern void* handler_connection(void*);

void controller_start()
{
	mf_logger_open(mf_default_log_path);
	listen_sockfd = mf_listen_socket_create();
	mf_socket_bind(listen_sockfd);

	signal(SIGPIPE,SIG_IGN);
	pthread_t thread_id[NET_RECEIVE_WORKER_NUMBER];
	int i = 0;
	for(; i < NET_RECEIVE_WORKER_NUMBER; i++)
	{
		if((pthread_create(&thread_id[i], 0, handle_connection, (void*)(&listen_sockfd))) < 0)
		{
			log_err("thread create error");
			exit(0);
		}
		pthread_detach(thread_id[i]);
	}
}
	
void controller_exit()
{
	mf_logger_close();
	mf_topomgr_destroy();
	mf_devicemgr_destroy();
	log_info("Microflow exit");
}

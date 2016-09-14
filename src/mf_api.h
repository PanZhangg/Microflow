#ifndef MF_API_H__
#define MF_API_H__

#include "mf_socket.h"
#include "mf_topomgr.h"
#include "mf_devicemgr.h"
#include "mf_msg_parser.h"
#include "mf_timer.h"
#include "mf_msg_handler.h"
#include "./httpserver/cgi.h"
#include "mf_controller.h"

void mf_controller_run()
{
	uint32_t listen_sockfd = mf_listen_socket_create();
	//printf("fd in main is: %d\n",listen_sockfd);
	mf_socket_bind(listen_sockfd);
	controller_start(&listen_sockfd);  
}

void mf_controller_init()
{
	parse_thread_start(WORKER_THREADS_NUM);
	start_stopwatch_thread();
	mf_devicemgr_create();
	mf_topomgr_create();
	msg_handlers_init();
	wr_init();
}

void register_msg_handler(enum MSG_HANDLER_TYPE type, msg_handler_func func)
{
	msg_handler_func_register(type, func);
}

void unregister_msg_handler(enum MSG_HANDLER_TYPE type, msg_handler_func func)
{
	msg_handler_func_unregister(type, func);
}

#endif

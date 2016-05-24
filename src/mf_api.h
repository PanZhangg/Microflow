#ifndef MF_API_H__
#define MF_API_H__


#include "mf_socket.h"
#include "mf_topomgr.h"
#include "mf_devicemgr.h"
#include "mf_msg_parser.h"
#include "mf_timer.h"
#include "mf_msg_handler.h"

void mf_controller_init()
{
	parse_thread_start(WORKER_THREADS_NUM);
	start_stopwatch_thread();
	mf_devicemgr_create();
	mf_topomgr_create();
	msg_handlers_init();
}

#endif

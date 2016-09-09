#include "mf_controller.h"
#include "dbg.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

void controller_start(uint32_t * sock)
{
	signal(SIGPIPE,SIG_IGN);
	pthread_t thread_id;
	if((pthread_create(&thread_id, 0, handle_connection, (void*)sock)) < 0)
	{
		log_err("thread create error");
		exit(0);
	}
	pthread_detach(thread_id);

}

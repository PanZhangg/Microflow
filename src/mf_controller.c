#include "mf_controller.h"

void controller_start(uint32_t * sock)
{
	uint32_t sockfd = *sock;
	//printf("fd in controller start is: %d\n",sockfd);
	pthread_t thread_id;
	if((pthread_create(&thread_id, 0, handle_connection, (void*)sock)) < 0)
	{
		perror("thread create error");
		exit(0);
	}
	pthread_detach(thread_id);

}

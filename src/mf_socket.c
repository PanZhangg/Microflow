#include "mf_socket.h"
#include "mf_msg_parser.h"
#include "mf_logger.h"
#include "mf_switch.h"
#include "mf_devicemgr.h"
#include "mf_rx_queue.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

struct sockaddr_in controller_addr, switch_addr;
struct epoll_event ev, events[EPOLL_EVENTS_NUM];

struct mf_rx_queue* MSG_RX_QUEUE;
uint32_t epfd, nfds;

char rx_buffer[4096];

static void set_nonblocking(uint32_t sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}


uint32_t mf_listen_socket_create()
{
	uint32_t sock;
	if((sock = socket(AF_INET, SOCK_STREAM,0)) < 0){
		perror("socket created failed");
		exit(0);
	}
	mf_write_socket_log("controller socket created",sock);
	return sock;
}

static void epoll_init(uint32_t sock)
{
	epfd = epoll_create(1);
	ev.data.fd = sock;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
	set_nonblocking(sock);
	listen(sock, SOMAXCONN);
}


void mf_socket_bind(uint32_t sock)
{
	memset(&controller_addr, 0, sizeof(controller_addr));
	controller_addr.sin_family = AF_INET;
	controller_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	controller_addr.sin_port = htons(DEFAULT_PORT);
	if((bind(sock, (struct sockaddr*)&controller_addr,sizeof(controller_addr))) == -1)
	{
		perror("socket bind failed");
		exit(0);
	}
	mf_write_socket_log("socket binded with local IP addresses",sock);
}


void handle_connection(uint32_t sock)
{
	int i, connfd;
	socklen_t clilen;
	epoll_init(sock);
	MSG_RX_QUEUE = mf_rx_queue_init();
	while(1)
	{
		nfds = epoll_wait(epfd, events, EPOLL_EVENTS_NUM, 100);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == sock)
			{
				printf("incoming connection\n");
				connfd = accept(sock, (struct sockaddr*)&switch_addr, &clilen);
				mf_write_socket_log("Incoming socket connection", connfd);
				if(connfd < 0)
				{
                    perror("connfd<0");
                    continue;
				}
				mf_switch_create(connfd);
				parse_thread_start(WORKER_THREADS_NUM);
				ev.data.fd = connfd;
				ev.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}
			else if(events[i].events & EPOLLIN)
			{
				int sockfd = events[i].data.fd;
				if(sockfd < 0)
				{
					perror("sockfd < 0");
					continue;
				}
				struct mf_switch * sw = get_switch(sockfd);
				memset(rx_buffer, 0, 4096);
				int length = read(sockfd, rx_buffer, 4096);
				if(length == 0)
				{
					mf_switch_destory(sw);
					mf_write_socket_log("socket closed", sockfd);
					printf("socket closed\n");	
					continue;
				}
				if(length < 0)
				{
					printf("socket error\n");
					continue;
				}
				else
				{
					/*TO DO 
					Make a mem pool for recv messages
					re-alloc mem when the pool is full**/
					char* node_rx_buffer = (char*)malloc(length);
					memcpy(node_rx_buffer,rx_buffer, length);
					struct q_node* qn = q_node_init(node_rx_buffer, length, sw);
					if(push_q_node(qn, MSG_RX_QUEUE) == 0)
						printf("queue push error");
				}
			}
		}
	}
}

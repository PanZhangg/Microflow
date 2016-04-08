#include "mf_socket.h"
#include "mf_msg_parser.h"
#include "mf_logger.h"
#include "mf_switch.h"
#include "mf_devicemgr.h"
#include "mf_rx_queue.h"
#include "mf_mempool.h"
#include "mf_timer.h"
#include "mf_utilities.h"
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
struct mf_queue_node_mempool * MSG_RX_QUEUE;
uint32_t epfd, nfds;

#define RX_BUFFER_SIZE 2048


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
	char rx_buffer[RX_BUFFER_SIZE];
	epoll_init(sock);
	MSG_RX_QUEUE = mf_queue_node_mempool_create();
	parse_thread_start(WORKER_THREADS_NUM);
	start_stopwatch_thread();
	mf_devicemgr_create();
	while(1)
	{
		nfds = epoll_wait(epfd, events, EPOLL_EVENTS_NUM, -1);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == sock)
			{
				printf("incoming connection\n");
				connfd = accept(sock, (struct sockaddr*)&switch_addr, &clilen);
				mf_write_socket_log("Incoming socket connection", connfd);
				if(connfd < 0)
				{
                    perror("connfd < 0");
                    continue;
				}
				mf_switch_create(connfd);
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
				int length = read(sockfd, rx_buffer, RX_BUFFER_SIZE);
				if(length == 0)
				{
					ev.data.fd = sockfd;
					ev.events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					mf_switch_destory(sw);
					mf_write_socket_log("socket closed", sockfd);
					printf("socket closed\n");	
					continue;
				}
				if(length < 0)
				{
					printf("socket error\n");
					mf_write_socket_log("socket error", sockfd);
					continue;
				}
				else
				{
					mf_write_socket_log("Message in", sockfd);
					/*Sometimes epoll reads in two or more openflow messages once.
					Use length field of openflow header to tell if all the messages
					are pushed to mempool respectively*/
					char * pkt_prt = rx_buffer;
					while(length != 0)
					{
						uint16_t msg_length;
						inverse_memcpy(&msg_length, pkt_prt + 2, 2);
						push_queue_node_to_mempool(pkt_prt, msg_length, sw, MSG_RX_QUEUE);
						pkt_prt += msg_length;
						length -= msg_length;
					}	
				}
			}
		}
	}
}

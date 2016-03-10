#include "mf_socket.h"
#include "mf_logger.h"
#include "mf_switch.h"

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
#include <pthread.h>

struct sockaddr_in controller_addr, switch_addr;
struct epoll_event ev, events[EPOLL_EVENTS_NUM];

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
	epfd = epoll_create(256);
	ev.data.fd = sock;
	ev.events = EPOLLIN | EPOLLET;
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
	struct mf_switch * sw;
	while(1)
	{
		nfds = epoll_wait(epfd, events, 4096, -1);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == sock)
			{
				printf("incoming connection\n");
				mf_write_socket_log("Incoming socket connection", sock);
				connfd = accept(sock, (struct sockaddr*)&switch_addr, &clilen);
				if(connfd < 0)
				{
                    perror("connfd<0");
                    continue;
				}
				sw = mf_switch_create(connfd);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
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
				memset(rx_buffer, 0, 4096);
				int length = read(sockfd, rx_buffer, 4096);
				if(length == 0)
				{
			
					close(sockfd);
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
					char* node_rx_buffer = (char*)malloc(length);
					memcpy(node_rx_buffer,rx_buffer, length);
					struct q_node* qn = q_node_init(node_rx_buffer, length, sockfd);
					struct mf_rx_queue* rxq = sw->rxq;
					//struct mf_rx_queue* rxq = get_rx_queue(sockfd, mf_socket_array);
					if(push_q_node(qn, rxq) == 0)
						printf("queue push error");
				}
			}
		}
	}
}


/*void destory_mf_socket(struct mf_socket s)
{
	while(s.rx_queue->queue_length);
	destory_queue(s.rx_queue);
}*/
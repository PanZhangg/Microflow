#include "mf_socket.h"
#include "mf_logger.h"
#include "mf_switch.h"
#include "mf_rx_queue.h"
#include "mf_mempool.h"
#include "mf_utilities.h"
#include "mf_api.h"
#include "dbg.h"
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
#include <netinet/tcp.h>

struct sockaddr_in controller_addr, switch_addr;
struct epoll_event ev, events[EPOLL_EVENTS_NUM];
struct mf_queue_node_mempool * MSG_RX_QUEUE[WORKER_THREADS_NUM];
uint32_t epfd, nfds;

static void set_nonblocking(uint32_t sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        log_err("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        log_err("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}


uint32_t mf_listen_socket_create()
{
	uint32_t sock;
	if((sock = socket(AF_INET, SOCK_STREAM,0)) == -1){
		log_err("socket created failed");
		exit(0);
	}
	int enable = 1;
	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable)) != 0)
	{
		log_err("set sock option failed");
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
		log_err("socket bind failed");
		exit(0);
	}
	mf_write_socket_log("socket binded with local IP addresses",sock);
}


void* handle_connection(void* arg)
{
	uint32_t sock = *(uint32_t*)arg;
	//printf("fd in connection is: %d\n",sock);
	int cpunum = sysconf(_SC_NPROCESSORS_ONLN);
	if(cpunum >= 4)
	{
		int ccpu_id = 0;
		cpu_set_t my_set;
		CPU_ZERO(&my_set);
		CPU_SET(ccpu_id, &my_set);
		if(sched_setaffinity(0, sizeof(cpu_set_t), &my_set) == -1)
			log_warn("Set CPU affinity failed");
	}
	unsigned int i;
	int connfd;
	socklen_t clilen = sizeof(switch_addr);
	epoll_init(sock);
	static unsigned int seq = 0;
	for(i = 0; i < WORKER_THREADS_NUM; i++)
	{
		MSG_RX_QUEUE[i] = mf_queue_node_mempool_create();
	}
	mf_controller_init();
	while(1)
	{
		nfds = epoll_wait(epfd, events, EPOLL_EVENTS_NUM, -1);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == sock)
			{
				log_info("incoming connection");
				connfd = accept(sock, (struct sockaddr*)&switch_addr, &clilen);
				mf_write_socket_log("Incoming socket connection", connfd);
				if(connfd < 0)
				{
					log_warn("connfd < 0");
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
					log_warn("sockfd < 0");
					continue;
				}
				struct mf_switch * sw = get_switch(sockfd);
				int length = read(sockfd, (char*)(sw->rx_buffer) + sw->epoll_recv_incomplete_length, RX_BUFFER_SIZE);
				if(length == 0)
				{
					ev.data.fd = sockfd;
					ev.events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					mf_switch_destory(sw);
					mf_write_socket_log("socket closed", sockfd);
					log_info("socket closed");	
					continue;
				}
				if(length < 0)
				{
					log_warn("socket error\n");
					mf_write_socket_log("socket error", sockfd);
					continue;
				}
				else if(likely(length > 0))
				{
					char * pkt_ptr = sw->rx_buffer;
					length += sw->epoll_recv_incomplete_length;
					sw->epoll_recv_incomplete_length = 0;
					int received_length = length;
					while(length > 0)
					{
						if(likely((int)*pkt_ptr == 4))// This is a OF1.3 Msg
						{
							//uint16_t msg_length = (uint16_t)*(pkt_ptr + 3) << 8 | *(pkt_ptr + 2);
							uint16_t msg_length = 0;
							inverse_memcpy(&msg_length, pkt_ptr + 2, 2);
							if(unlikely(length < msg_length))
							{
								log_warn("received length is: %d,current length is: %d\n,  msg length is %d\n",received_length, length, msg_length);
								sw->epoll_recv_incomplete_length = length;
								memmove(sw->rx_buffer, pkt_ptr, sw->epoll_recv_incomplete_length);
								break;	
							}
							push_queue_node_to_mempool(pkt_ptr, msg_length, sw, MSG_RX_QUEUE[(seq++) % WORKER_THREADS_NUM]);
							pkt_ptr += msg_length;
							length -= msg_length;
						}
						else
						{
							log_warn("Msg is not OF1.3");
							log_warn("Packets drop, total length: %d\n", length);
							break;
						}
					}
				}
			}
		}
	}
}

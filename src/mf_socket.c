#include "mf_socket.h"
#include "mf_socket_array.h"
#include "mf_msg_parser.h"
#include "mf_logger.h"

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
extern pthread_mutex_t socket_array_mutex;
//struct mf_socket mf_socket_array[4096];
struct mf_socket_array * mf_socket_array;
struct sockaddr_in controller_addr, switch_addr;
//memset(&controller_addr, 0, sizeof(controller_addr));
struct epoll_event ev, events[EPOLL_EVENTS_NUM];

uint32_t epfd, nfds;

char rx_buffer[4096];

static void set_nonblocking(int sock)
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


struct mf_socket mf_listen_socket_create()
{
	struct mf_socket s;
	if((s.socket_fd = socket(AF_INET, SOCK_STREAM,0)) == -1){
		perror("socket created failed");
		exit(0);
	}
	s.rx_queue = mf_rx_queue_init();
	//printf("\nlisten socket created\n");
	mf_write_socket_log("controller socket created",s.socket_fd);
	return s;
}

static void epoll_init(struct mf_socket s)
{
	epfd = epoll_create(256);
	ev.data.fd = s.socket_fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, s.socket_fd, &ev);
	//printf("\nepoll inited for socket%d\n",s.socket_fd);
	set_nonblocking(s.socket_fd);
	listen(s.socket_fd, SOMAXCONN);
	//printf("\nlistening at socket %d\n",s.socket_fd);
}

struct mf_socket mf_socket_create(uint32_t fd)
{
	struct mf_socket s;
	s.socket_fd = fd;
	s.rx_queue = mf_rx_queue_init();
	return s;
}

void mf_socket_bind(struct mf_socket s)
{
	memset(&controller_addr, 0, sizeof(controller_addr));
	controller_addr.sin_family = AF_INET;
	controller_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	controller_addr.sin_port = htons(DEFAULT_PORT);
	if((bind(s.socket_fd, (struct sockaddr*)&controller_addr,sizeof(controller_addr))) == -1)
	{
		perror("socket bind failed");
		exit(0);
	}
	mf_write_socket_log("socket binded with local IP addresses",s.socket_fd);
	//printf("\nsocket binded, IP:%u, Port:%d\n",ntohl(controller_addr.sin_addr.s_addr), ntohs(controller_addr.sin_port));
}


void handle_connection(struct mf_socket s)
{
	int i, connfd;
	socklen_t clilen;
	epoll_init(s);
	mf_socket_array = mf_socket_array_init();
	parser_thread_start();
	while(1)
	{
		nfds = epoll_wait(epfd, events, 2000000, -1);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == s.socket_fd)
			{
				printf("incoming connection\n");
				mf_write_socket_log("Incoming socket connection", s.socket_fd);
				connfd = accept(s.socket_fd, (struct sockaddr*)&switch_addr, &clilen);
				//printf("\nconnfd: %d", connfd);
				if(connfd<0)
				{
                    perror("connfd<0");
                    //mf_write_socket_log("connfd < 0", s.socket_fd);
                    exit(1);
				}
				//mf_socket_array[connfd] = mf_socket_create(connfd);
				struct mf_socket sk = mf_socket_create(connfd);
				struct mf_socket_array_node* san = mf_socket_array_node_init(sk);
				pthread_mutex_lock(&socket_array_mutex);
				if(insert_mf_socket_array(san,mf_socket_array) != 1)
				{
					perror("insert socket error");
					exit(1);
				}
				pthread_mutex_unlock(&socket_array_mutex);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}
			else if(events[i].events & EPOLLIN)
			{
				int sockfd = events[i].data.fd;
				//printf("\nsockfd: %d", sockfd);
				if(sockfd < 0)
					break;
				memset(rx_buffer, 0, 4096);
				int length = read(sockfd, rx_buffer, 4096);
				if(length == 0)
				{
					pthread_mutex_lock(&socket_array_mutex);
					if(delete_socket_array_node(sockfd, mf_socket_array) == 0)
					{
						perror("can not delete socket array node");
						break;
					}
					close(sockfd);
					mf_write_socket_log("socket closed", sockfd);
					printf("socket closed\n");
					pthread_mutex_unlock(&socket_array_mutex);
					break;
				}
				if(length < 0 )
					break;
				char* node_rx_buffer = (char*)malloc(length);
				memcpy(node_rx_buffer,rx_buffer, length);
				struct q_node* qn = q_node_init(node_rx_buffer, length, sockfd);
				struct mf_rx_queue* rxq = get_rx_queue(sockfd, mf_socket_array);
				if(push_q_node(qn, rxq) == 0)
					continue;
			}
		}
	}
}


void destory_mf_socket(struct mf_socket s)
{
	destory_queue(s.rx_queue);
}
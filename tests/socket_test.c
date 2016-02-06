#include "../src/mf_socket.h"



//extern struct mf_socket mf_socket_array[4096];

//extern struct sockaddr_in controller_addr, switch_addr;
//memset(&controller_addr, 0, sizeof(controller_addr));
//extern struct epoll_event ev; 
//extern struct epoll_event events[EPOLL_EVENTS_NUM];

//extern uint32_t epfd, nfds;

//extern char rx_buffer[4096];


int main(int argc, char** argv){
	struct mf_socket s = mf_listen_socket_create();
	mf_socket_bind(s);
	handle_connection(s);
	return 0;
}
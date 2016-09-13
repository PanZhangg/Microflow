#include <sched.h>
#include "mf_socket.h"
#include "mf_controller.h"
#include "mf_logger.h"
#include "stdio.h"
#include "dbg.h"
#include <string.h>

//uint32_t listen_sockfd = 0;
static void print_info()
{
	printf("\n\n		-=Welcome to Microflow=-\n");
	printf("		-=PanZhang dazhangpan@gmail.com=-\n");
	printf("		-=WebUI://localhost:8000=-\n");
}

void print_other()
{
	printf("To quit, type \033[1m'quit'\033[m"); 
	printf("\n");
}

void print_welcome()
{
	print_info();
	printf("	\033[31m __  __ _                ______ _               \n\033[0m");
	printf("	\033[31m|  \\/  (_)              |  ____| |              \n\033[0m");
	printf("	\033[31m| \\  / |_  ___ _ __ ___ | |__  | | _____      __\n\033[0m");
	printf("	\033[31m| |\\/| | |/ __| '__/ _ \\|  __| | |/ _ \\ \\ /\\ / /\n\033[0m");
	printf("	\033[31m| |  | | | (__| | | (_) | |    | | (_) \\ V  V / \n\033[0m");
	printf("	\033[31m|_|  |_|_|\\___|_|  \\___/|_|    |_|\\___/ \\_/\\_/  \n\033[0m");
print_other();
}



int main(int argc, char** argv)
{
	char buf[32];
	print_welcome();
	mf_logger_open(mf_default_log_path);
	uint32_t listen_sockfd = mf_listen_socket_create();
	//printf("fd in main is: %d\n",listen_sockfd);
	mf_socket_bind(listen_sockfd);
	controller_start(&listen_sockfd);
	while(scanf("%s", buf) > 0)
	{
		if(strcmp("quit", buf) == 0)
			break;
	}
	mf_logger_close();
	controller_exit();
	return 0;
}

#include <sched.h>
#include "../src/mf_socket.h"
#include "../src/mf_logger.h"
#include "stdio.h"

void print_welcome()
{
	printf("\n\033[31m Starting Microflow..\n\n\033[0m");
	printf("	\033[31m __  __ _                ______ _               \n\033[0m");
	printf("	\033[31m|  \\/  (_)              |  ____| |              \n\033[0m");
	printf("	\033[31m| \\  / |_  ___ _ __ ___ | |__  | | _____      __\n\033[0m");
	printf("	\033[31m| |\\/| | |/ __| '__/ _ \\|  __| | |/ _ \\ \\ /\\ / /\n\033[0m");
	printf("	\033[31m| |  | | | (__| | | (_) | |    | | (_) \\ V  V / \n\033[0m");
	printf("	\033[31m|_|  |_|_|\\___|_|  \\___/|_|    |_|\\___/ \\_/\\_/  \n\033[0m");
}

int main(int argc, char** argv){
	print_welcome();
	mf_logger_open(mf_default_log_path);
	mf_write_log("--Test starts here--");
	uint32_t s = mf_listen_socket_create();
	mf_socket_bind(s);
	//char command[60];
	handle_connection(s);
	mf_logger_close();
	return 0;
}

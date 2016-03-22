#include "../src/mf_socket.h"
#include "../src/mf_logger.h"
#include <malloc.h>


int main(int argc, char** argv){
	mallopt(M_ARENA_MAX, 1);
	mf_logger_open(mf_default_log_path);
	mf_write_log("--Test starts here--");
	uint32_t s = mf_listen_socket_create();
	mf_socket_bind(s);
	//char command[60];
	handle_connection(s);
	mf_logger_close();
	return 0;
}
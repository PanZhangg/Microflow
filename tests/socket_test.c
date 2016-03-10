#include "../src/mf_socket.h"

#include "../src/mf_logger.h"


int main(int argc, char** argv){
	mf_logger_open(mf_default_log_path);
	mf_write_log("--Test starts here--");
	//mf_logger_close();
	//extern struct mf_socket_array* mf_socket_array;
	uint32_t s = mf_listen_socket_create();
	mf_socket_bind(s);
	handle_connection(s);
	mf_logger_close();
	return 0;
}
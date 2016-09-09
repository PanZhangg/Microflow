#include "mf_logger.h"
#include "dbg.h"
#include "mf_timer.h"

#include <stdint.h>
#include <stdlib.h>

FILE* MF_LOG_FILE;

void mf_logger_open(const char* path)
{
	if(path == NULL)
	{
		log_warn("Log path is not specific");
		path = mf_default_log_path;
		//exit(0);
	}

	MF_LOG_FILE = fopen(path, "w");
	if(MF_LOG_FILE == NULL)
	{
		log_err("File open failed");
		exit(0);
	}
}

void mf_write_log(char* msg)
{
	if(msg == NULL)
	{
		log_warn("Log Msg is null");
		//exit(0);
	}
	char* t = get_asctime();
	if(fprintf(MF_LOG_FILE, "[%s]:%s\n", t, msg) < 0)
	{
		log_warn("Log write error");
	}	
}

void mf_write_socket_log(char* msg, int socketfd)
{
	if(msg == NULL)
	{
		log_warn("\nLog Msg is null");
		//exit(0);
	}
	char* t = get_asctime();
	if(fprintf(MF_LOG_FILE, "[%s]:<Socket: %d> %s\n", t, socketfd, msg) < 0)
	{
		log_warn("Socket log write error");
	}
}

void mf_logger_close()
{
	fclose(MF_LOG_FILE);
}

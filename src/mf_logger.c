#include "mf_logger.h"
#include "mf_timer.h"

#include <stdint.h>
#include <stdlib.h>

FILE* MF_LOG_FILE;

void mf_logger_open(const char* path)
{
	if(path == NULL)
	{
		printf("\nLog path is not specific");
		path = mf_default_log_path;
		//exit(0);
	}

	MF_LOG_FILE = fopen(path, "w");
	if(MF_LOG_FILE == NULL)
	{
		printf("\nFile open failed");
		exit(0);
	}
}

void mf_write_log(char* msg)
{
	if(msg == NULL)
	{
		printf("\nLog Msg is null");
		//exit(0);
	}
	char* t = get_asctime();
	if(fprintf(MF_LOG_FILE, "[%s]:%s\n", t, msg) < 0)
	{
		printf("Log write error\n");
	}	
}

void mf_write_socket_log(char* msg, int socketfd)
{
	if(msg == NULL)
	{
		printf("\nLog Msg is null");
		//exit(0);
	}
	char* t = get_asctime();
	if(fprintf(MF_LOG_FILE, "[%s]:<Socket: %d> %s\n", t, socketfd, msg) < 0)
	{
		printf("Socket log write error\n");
	}
}

void mf_logger_close()
{
	fclose(MF_LOG_FILE);
}
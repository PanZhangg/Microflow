#include "mf_logger.h"
#include "mf_timer.h"

void mf_logger_open(const char* path)
{
	if(path == NULL)
	{
		printf("\nLog path is not specific");
		exit(0);
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
		printf("Log write error");
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
	if(fprintf(MF_LOG_FILE, "[%s]:<Socket: %d>, %s\n", t, socketfd, msg) < 0)
	{
		printf("Log write error");
	}
}

void mf_logger_close()
{
	fclose(MF_LOG_FILE);
}
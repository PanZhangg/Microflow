#include "mf_logger.h"
#include <string.h>

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
	//fprintf(MF_LOG_FILE, )
	//printf("\nmsg length: %d",(int)strlen(msg));
	fprintf(MF_LOG_FILE, "%s\n", msg);	
}

void mf_logger_close()
{
	fclose(MF_LOG_FILE);
}
#ifndef __MF_LOGGER_H__
#define __MF_LOGGER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define mf_default_log_path "./microflow.log"
//const char* mf_default_log_path = "microflow.log";
FILE* MF_LOG_FILE;

void mf_logger_open(const char* path);
void mf_write_log(char*);
void mf_write_socket_log(char*, int);
void mf_logger_close();

#endif

#ifndef __MF_LOGGER_H__
#define __MF_LOGGER_H__

#include <stdio.h>
#include <pthread.h>

#define mf_default_log_path "./microflow.log"
extern FILE* MF_LOG_FILE;
pthread_mutex_t log_mutex;

void mf_logger_open(const char* path);
void mf_write_log(char*);
void mf_write_socket_log(char*, int);
void mf_logger_close();

#endif

#include "mf_switch.h"
#include "mf_msg_parser.h"
#include "mf_logger.h"
#include "mf_devicemgr.h"
#include "mf_rx_queue.h"
#include "dbg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct mf_switch * mf_switch_create(uint32_t sockfd)
{
	struct mf_switch * sw = (struct mf_switch*)malloc(sizeof(struct mf_switch));
	if(sw == NULL)
	{
		log_err("malloc failed");
		exit(0);
	}
	sw->sockfd = sockfd;
	sw->epoll_recv_incomplete_length = 0;
	sw->datapath_id = 0;
	sw->n_buffers = 0;
	sw->n_tables = 0;
	sw->auxiliary_id = 0;
	sw->capabilities = 0;
	sw->port_num = 0;
	memset(&(sw->ports), 0, sizeof(sw->ports));
	sw->is_hello_sent = 0;
	sw->is_feature_request_sent = 0;
	sw->is_port_desc_request_sent = 0;
	sw->feature_request_xid = 0;
	/*link_list has already been assigned an address*/
	add_switch(sw);
	pthread_mutex_init(&(sw->switch_mutex), NULL);
	return sw;
}

void mf_switch_destory(struct mf_switch * sw)
{
	if(sw == NULL)
	{
		log_warn("error: switch to destory is NULL");
		//exit(0);
		return;
	}
	if(close(sw->sockfd) < 0)
	{
		log_warn("socket close error");
	}
	delete_switch_from_map(sw);
	if(pthread_mutex_destroy(&sw->switch_mutex) < 0)
		log_warn("mutex destroy error");
	free(sw);
	sw = NULL;
}


void switch_print(struct mf_switch * sw)
{
	printf("sockfd:%d\n", sw->sockfd); 
	printf("datapath_ID:%ld\n", sw->datapath_id); 
	printf("port num:%d\n", sw->port_num); 
	int i = 0;
	for(i = 0; i < sw->port_num; i++)
	{
  		printf("Port %d: Port_no: %x\n", i, sw->ports[i].port_no);	
  		printf("Port %d: hardware addr: %x", i, (char)sw->ports[i].hw_addr[5]);	
  		printf(":%x", (char)sw->ports[i].hw_addr[4]);	
  		printf(":%x", (char)sw->ports[i].hw_addr[3]);	
  		printf(":%x", (char)sw->ports[i].hw_addr[2]);	
  		printf(":%x", (char)sw->ports[i].hw_addr[1]);	
  		printf(":%x\n", (char)sw->ports[i].hw_addr[0]);	
	}
}


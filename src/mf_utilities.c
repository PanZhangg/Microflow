#include "mf_utilities.h"
#include "mf_switch.h"
#include "dbg.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>

/*==========================
Generate an uint32_t random number
seed is the current system time(usec)
===========================*/

uint32_t generate_random()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	uint32_t r = rand_r((unsigned int *)&(t.tv_usec));
	return r;
}

inline void mf_send(struct mf_switch * sw, void * msg, uint32_t length)
{
	send(sw->sockfd, msg, length, MSG_DONTWAIT);
}
/*==========================
Copy len byte of data from memory pointed 
by src inversely to memory pointed by dst
e.g. src = "abcd", dst = "dcba"
===========================*/
inline void inverse_memcpy(void* dst, void* src, uint16_t len)
{
		uint16_t i = 0;
		src = (char*)src + len - 1;
		while(i < len)
		{
			*(char*)dst++ = *(char*)src--;
			i++;
		}
}

inline uint16_t copy_16bit(char * ptr)
{
	return *(uint16_t *)ptr;
}

inline uint32_t copy_32bit(char * ptr)
{
	return *(uint32_t *)ptr;
}

inline uint64_t copy_64bit(char * ptr)
{
	return *(uint64_t *)ptr;
}

inline uint32_t swap_32bit(uint32_t a)
{
	uint32_t rst;
	asm("movl %1, %%eax;bswap %%eax;movl %%eax,%0;":"=r"(rst):"r"(a):"%eax");
	return rst;
}

inline uint16_t swap_16bit(uint16_t a)
{
	uint16_t rst;
	asm("mov %1, %%ax;xchg %%al, %%ah;mov %%ax, %0;":"=r"(rst):"r"(a):"%ah");
	return rst;
}

inline uint16_t ntoh_16bit(char * ptr)
{
	return swap_16bit(copy_16bit(ptr));
}

inline uint32_t ntoh_32bit(char * ptr)
{
 	return swap_32bit(copy_32bit(ptr));
}

inline uint64_t ntoh_64bit(char * ptr)
{
	uint64_t r = (uint64_t)swap_32bit(copy_32bit(ptr))<< 32;
	return ( r | swap_32bit(copy_32bit(ptr + 4)));
}
  

void set_cpu_affinity()
{
	int cpunum = sysconf(_SC_NPROCESSORS_ONLN);
	static char cpu_usage_flag[64];
	if(cpunum >= 4)
	{
		int i = 0;
		for(; i < cpunum; i++)
		{
			if(cpu_usage_flag[i] == 0)
			{
				cpu_usage_flag[i] = 1;
				break;
			}
  		}
		int ccpu_id = i;
		cpu_set_t my_set;
		CPU_ZERO(&my_set);
		CPU_SET(ccpu_id, &my_set);
		if(sched_setaffinity(0, sizeof(cpu_set_t), &my_set) == -1)
			log_warn("Set CPU affinity failed");
		log_info("Set CPU Affinity of pthread:[%ld],to Core:[%d]",(long int)syscall(SYS_gettid), ccpu_id);
	}
	else
		log_info("Number of CPU cores is less than 4, Can not set CPU affinity");
}

static UINT8 gn_htonll(UINT8 n)
{
    return htonl(1) == 1 ? n : ((UINT8) htonl(n) << 32) | htonl(n >> 32);
}

void create_lldp_pkt(void *src_addr, UINT8 id, ovs_be32 port, lldp_t *buffer)
{
	if(src_addr == NULL)
		return;
	UINT1 dest_addr[6] = {0x01,0x80,0xc2,0x00,0x00,0x0e};

	memcpy(buffer->eth_head.dest,dest_addr,6);
	memcpy(buffer->eth_head.src, src_addr, 6);
	buffer->eth_head.proto = htons(0x88cc);

	buffer->chassis_tlv_type_and_len = htons(0x0209);
	buffer->chassis_tlv_subtype = LLDP_CHASSIS_ID_LOCALLY_ASSIGNED;
	buffer->chassis_tlv_id = gn_htonll(id);   //datapath id

	buffer->port_tlv_type_and_len    = htons(0x0403);
	buffer->port_tlv_subtype = LLDP_PORT_ID_COMPONENT;
	buffer->port_tlv_id = htons(port);         //send port

	buffer->ttl_tlv_type_and_len = htons(0x0602);
	buffer->ttl_tlv_ttl = htons(120);

	/*
	   buffer->orgspe_tlv_type_and_len = htons(0xfe0c);
	   buffer->unique_code[0]          = 0x00;
	   buffer->unique_code[1]          = 0x26;
	   buffer->unique_code[2]          = 0xe1;
	   memcpy(buffer->sub_content, sub_content, 9);

	   buffer->unknow1_tlv_type_and_len = htons(0x1808);
	   buffer->unknow1_code             = htonll(0x0baa92b47d87cbba);

	   buffer->unknow2_tlv_type_and_len = htons(0xe601);
	   buffer->unknow2_code             = 0x01;
	 */

	buffer->endof_lldpdu_tlv_type_and_len = 0x00;
}

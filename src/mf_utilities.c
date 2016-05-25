#include "mf_utilities.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

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
/*==========================
Copy len byte of data from memory pointed 
by src inversely to memory pointed by dst
e.g. src = "abcd", dst = "dcba"
===========================*/
void inline inverse_memcpy(void* dst, void* src, uint16_t len)
{
	if(len <= 0 || dst == NULL || src == NULL)
		return;
	else
	{
		uint16_t i = 0;
		src = (char*)src + len - 1;
		while(i < len)
		{
			*(char*)dst++ = *(char*)src--;
			i++;
		}
	}
}

static UINT8 gn_htonll(UINT8 n)
{
    return htonl(1) == 1 ? n : ((UINT8) htonl(n) << 32) | htonl(n >> 32);
}

void create_lldp_pkt(void *src_addr, UINT8 id, UINT2 port, lldp_t *buffer)
{
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
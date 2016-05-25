#ifndef MF_UTILITIES_H__
#define MF_UTILITIES_H__

#include "Openflow/types.h"

/*
program utilities
*/
uint32_t generate_random();
void inverse_memcpy(void* dst, void* src, uint16_t len);

/*
Network packets utilities
*/

typedef signed   char     BOOL;
typedef char              INT1;
typedef short             INT2;
typedef int               INT4;
typedef unsigned char     UINT1;
typedef unsigned short    UINT2;
typedef unsigned int      UINT4;
typedef unsigned int      UINT;
/*
64bit Linux OS only
*/
typedef long              INT8;
typedef unsigned long     UINT8;

#define MAX_PKT 1600


enum ether_type
{
    ETHER_LLDP = 0x88cc,
    ETHER_ARP = 0x0806,
    ETHER_IP = 0x0800,
    ETHER_IPV6 = 0x86DD,
    ETHER_VLAN = 0x8100,
    ETHER_MPLS = 0x8847
};
enum lldp_tlv_type{
    /* start of mandatory TLV */
    LLDP_END_OF_LLDPDU_TLV = 0,
    LLDP_CHASSIS_ID_TLV = 1,
    LLDP_PORT_ID_TLV = 2,
    LLDP_TTL_TLV = 3,
    /* end of mandatory TLV */
    /* start of optional TLV */ /*NOT USED */
    LLDP_PORT_DESC_TLV = 4,
    LLDP_SYSTEM_NAME_TLV = 5,
    LLDP_SYSTEM_DESC_TLV = 6,
    LLDP_SYSTEM_CAPABILITY_TLV = 7,
    LLDP_MGMT_ADDR_TLV = 8
    /* end of optional TLV */
};

enum lldp_chassis_id_subtype {
    LLDP_CHASSIS_IP_MAC              = 4,
    LLDP_CHASSIS_ID_LOCALLY_ASSIGNED = 7
};

enum lldp_port_id_subtype {
    LLDP_PORT_ID_COMPONENT        = 2,
    LLDP_PORT_ID_LOCALLY_ASSIGNED = 7
};

#pragma pack(1)
typedef struct st_ether
{
    UINT1 dest[6];
    UINT1 src[6];
    UINT2 proto;
    UINT1 data[0];
}ether_t;

typedef struct st_arp
{
    ether_t eth_head;
    UINT2 hardwaretype;
    UINT2 prototype;
    UINT1 hardwaresize;
    UINT1 protocolsize;
    UINT2 opcode;
    UINT1 sendmac[6];
    UINT4 sendip;
    UINT1 targetmac[6];
    UINT4 targetip;
    UINT1 data[0];
}arp_t;

typedef struct st_ip
{
    ether_t eth_head;
    UINT1 hlen;
    UINT1 tos;
    UINT2 len;
    UINT2 ipid;
    UINT2 fragoff;
    UINT1 ttl;
    UINT1 proto;
    UINT2 cksum;
    UINT4 src;
    UINT4 dest;
    UINT1 data[0];
}ip_t;

typedef struct st_tcp
{
    UINT2 sport;
    UINT2 dport;
    UINT4 seq;
    UINT4 ack;
    UINT1 offset;
    UINT1 code;
    UINT2 window;
    UINT2 cksum;
    UINT2 urg;
    UINT1 data[0];
}tcp_t;

typedef struct st_udp
{
    UINT2 sport;
    UINT2 dport;
    UINT2 len;
    UINT2 cksum;
    UINT1 data[0];
}udp_t;

typedef struct st_imcp
{
    UINT1 type;
    UINT1 code;
    UINT2 cksum;
    UINT2 id;
    UINT2 seq;
    UINT1 data[0];
}icmp_t;

typedef struct st_t802_1q
{
    UINT2 proto;
    UINT2 vlan; 
    UINT1 data[0];
}t802_1q_t;

typedef struct st_dhcp
{
    UINT1 opt;
    UINT1 htype;
    UINT1 hlen;
    UINT1 hops;
    UINT4 xid;
    UINT2 secd;
    UINT2 flg;
    UINT4 cipaddr;  //client ip addr;
    UINT4 yipaddr;  //you ip addr;
    UINT4 sipaddr;  //server ip addr;
    UINT4 gipaddr;  //agent ip addr;
    UINT1 cmcaddr[16];
    UINT1 hostname[64];
    UINT1 filename[128];
    UINT4 sname;
    UINT1 data[0];
}dhcp_t;

typedef struct st_tlv
{
    UINT1 type;
    UINT1 len;
    char  data[0];
}tlv_t;

typedef struct st_dns_a
{
    UINT2 label_ptr;
    UINT2 type;         // type
    UINT2 clas;         // class
    UINT4 ttl;
    UINT2 len;
    UINT1  data[0];
}dns_a_t;

typedef struct st_dns_q
{
    UINT1   len;
    char    *data;
}dns_q_t;

typedef struct st_dns
{
    UINT2 id;
    UINT2 flags;
    UINT2 questions;
    UINT2 answer;
    UINT2 author;
    UINT2 add;
    dns_q_t  *dns_q;
}dns_t;

typedef struct st_ethpkt_info
{
    UINT1 data[MAX_PKT];
    UINT2 len;
    UINT2 vlan_id;
    INT4  qid;
    UINT1 ifindex;
    ether_t *pEth;
    ip_t *pIp;
    arp_t *pArp;
    tcp_t *pTcp;
    udp_t *pUdp;
    icmp_t *pIcmp;
    t802_1q_t *q8021q;
}ethpkt_info_t;

typedef struct st_lldp
{
    ether_t eth_head;

    //9 Byte
    UINT2 chassis_tlv_type_and_len;   //0x0207
    UINT1 chassis_tlv_subtype;       //MAC             4
    UINT8 chassis_tlv_id;            //dpid

    //5 Byte
    UINT2 port_tlv_type_and_len;      //0x0403
    UINT1 port_tlv_subtype;          //local assigned    7
    UINT2 port_tlv_id;                //send port

    //4 Byte
    UINT2 ttl_tlv_type_and_len;      //0x0602
    UINT2 ttl_tlv_ttl;               //ttl 120

    //2
    UINT2 endof_lldpdu_tlv_type_and_len;
}lldp_t;

#pragma pack()

void create_lldp_pkt(void *src_addr, UINT8 id, ovs_be32 port, lldp_t *buffer);

#endif

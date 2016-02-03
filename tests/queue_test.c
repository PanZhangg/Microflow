/* file minunit_example.c */
#include <stdio.h>
#include "../src/minunit.h"
#include "../src/mf_rx_queue.h"

char* packet_content1 = "abcdefg";
char* packet_content2 = "higklmn";
char* packet_content3 = "opqrstu";
uint8_t packet_length = 8;


static char* test_q_node_init(){
	struct q_node* test = q_node_init(packet_content1, packet_length);
	printf("\nPacket_content:%s\n",(char*)(test->rx_packet));
	mu_assert(test != NULL, "q_node_init_failed");
	destory_q_node(test);
	return NULL;
}

static char* test_push_q_node(){
	struct mf_rx_queue* queue = mf_rx_queue_init();
	printf("----here---");
	struct q_node* node1 = q_node_init(packet_content1, packet_length);
	mu_assert(push_q_node(node1, queue) == 1 ,"push queue failed");
	print_queue(queue);
	return NULL;
}




static char* test_handle_flow_mod()
{
	
	//pfm = parser_flow_mod_msg(flow_mod_message);
	//printf("\n%d\n",ntohs(pfm.ofm.priority));
	//mu_assert(init_flows() != NULL, "Init failed");
	return NULL; 
}

 char *all_tests() 
 {
    mu_suite_start();
    mu_run_test(test_q_node_init);
    mu_run_test(test_push_q_node);
    return NULL;
}

RUN_TESTS(all_tests);
 

#include "../src/mf_topomgr.h"
#include "../src/mf_switch.h"

int main()
{
	mf_topomgr_create();
	struct ofp11_port * sw1_port1;
	struct ofp11_port * sw1_port2;
	struct ofp11_port * sw2_port1;
	struct ofp11_port * sw2_port2;
	struct ofp11_port * sw3_port1;
	struct ofp11_port * sw3_port2;
	struct mf_switch* sw1 = mf_switch_create(6);
	struct mf_switch* sw2 = mf_switch_create(7);
	struct mf_switch* sw3 = mf_switch_create(8);
	sw1_port1 = &(sw1->ports[0]);
	sw1_port2 = &(sw1->ports[1]);
	sw2_port1 = &(sw2->ports[0]);
	sw2_port2 = &(sw2->ports[1]);
	sw3_port1 = &(sw3->ports[0]);
	sw3_port2 = &(sw3->ports[1]);

	struct link_node* src_node1 = link_node_create(sw1, sw1_port1);
	struct link_node* dst_node1 = link_node_create(sw2, sw2_port1);
	struct link_node* src_node2 = link_node_create(sw1, sw1_port2);
	struct link_node* dst_node2 = link_node_create(sw2, sw2_port2);
	struct link_node* src_node3 = link_node_create(sw1, sw1_port1);
	struct link_node* dst_node3 = link_node_create(sw3, sw3_port1);
	/*	int i = 0;
		for (i = 0; i < 256init cache array size - 6 - 1; i++)
		{
		link_node_create(sw1, &sw1_port1);
		}
	 */
	link_node_create(sw1, sw1_port1);
	link_node_create(sw1, sw1_port1);
	struct network_link * network_link1= network_link_create(src_node1, dst_node1);
	struct network_link * network_link2 = network_link_create(src_node2, dst_node2);
	struct network_link * network_link3 = network_link_create(src_node3, dst_node3);
	/*	for(i = 0; i < MAX_NETWORK_LINK_NUM; i++)
		network_link_create(src_node)
	 */
	sw_link_insert(&(sw1->link_list),network_link1);
	sw_link_insert(&(sw1->link_list),network_link2);
	/*struct link_list_element * link1 = link_list_element_create(network_link1);
	  struct link_list_element * link2 = link_list_element_create(network_link2);
	  struct link_list_element * link3 = link_list_element_create(network_link3);
	  struct path_link_list * path_link_list = path_link_list_create();
	  struct sw_link_list * sw_link_list = sw_link_list_create();
	  network_path_insert(path_link_list, link1);
	  network_path_insert(path_link_list, link2);
	  network_path_insert(path_link_list, link3);
	  path_link_delete(path_link_list, link1);
	  path_link_delete(path_link_list, link3);*/
	return 0;
}

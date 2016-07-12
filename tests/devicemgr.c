#include "../src/mf_devicemgr.h"
#include "../src/mf_switch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	struct mf_switch * sw1 = mf_switch_create(4);
	struct mf_switch * sw2 = mf_switch_create(5);
	struct mf_switch * sw3 = mf_switch_create(6);
	struct mf_switch * sw4 = mf_switch_create(7);
	struct mf_switch * sw5 = mf_switch_create(8);
	struct mf_switch * sw6,sw7,sw8,sw9,sw10;
	void mf_devicemgr_create();
	add_switch(sw1);
	add_switch(sw2);
	add_switch(sw3);
	add_switch(sw4);
	add_switch(sw5);
	int i,j = 0;
	for(; i < 5; i++)
	{
 		sw6 = get_next_switch(&j); 
	}
	uint64_t mac1 = 11111111111;
	uint64_t mac2 = 1111111111;
	mf_devicemgr_create();
	host_hash_value_add(sw6, 5, mac1);
	host_hash_value_add(sw6, 5, mac2);
	host_hash_value_add(sw6, 5, mac1);
	return 0;
}

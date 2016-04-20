#include "../src/mf_devicemgr.h"
#include "../src/mf_switch.h"

int main()
{
	uint64_t mac_addr1 = 0xacf01263b397;
	uint64_t mac_addr2 = 0x793b36210fca;
	uint32_t index1 = mac_addr_hash(mac_addr1);
	uint32_t index2 = mac_addr_hash(mac_addr2);
	struct mf_switch * sw1 = mf_switch_create(8);
	struct mf_switch * sw2 = mf_switch_create(9);
	struct host_hash_value * pvalue1 = host_hash_value_create(sw1, 1, mac_addr1);
	struct host_hash_value * pvalue2 = host_hash_value_create(sw2, 2, mac_addr2);
	host_add_to_hash_map(pvalue1);
	host_add_to_hash_map(pvalue1);
	host_add_to_hash_map(pvalue2);
	struct mf_switch * sw3 = get_switch_by_host_mac(mac_addr1);
	struct mf_switch * sw4 = get_switch_by_host_mac(mac_addr2);
	return 0;
}

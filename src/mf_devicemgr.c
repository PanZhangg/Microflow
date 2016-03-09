#include "mf_devicemgr.h"

struct mf_switch * mf_switch_create(struct mf_socket sk)
{
	struct mf_switch * sw = (struct mf_switch *)malloc(sizeof(*sw));
	sw->sw_socket = sk;

	return sw;
}
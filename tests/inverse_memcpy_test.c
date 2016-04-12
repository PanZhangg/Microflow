#include "../src/mf_utilities.h"

int main()
{
	char * src_buffer = "abcdefghijk";
	char dst_buffer[12];
	inverse_memcpy(dst_buffer, src_buffer, 11);
	return 0;
}
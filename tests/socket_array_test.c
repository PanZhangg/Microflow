#include <stdio.h>
#include "../src/mf_socket_array.h"
#include "../src/mf_socket.h"

#define FIRST_SOCKET_FD 5
#define SECOND_SOCKET_FD 6
#define THIRD_SOCKET_FD 7

int main(int argc, char** argv){
	struct mf_socket s1 = mf_socket_create(FIRST_SOCKET_FD);
	struct mf_socket_array_node* n1 = mf_socket_array_node_init(s1);
	struct mf_socket_array * q = mf_socket_array_init();
	insert_mf_socket_array(n1,q);
	struct mf_socket s2 = mf_socket_create(SECOND_SOCKET_FD);
	struct mf_socket_array_node* n2 = mf_socket_array_node_init(s2);
	insert_mf_socket_array(n2,q);
	struct mf_socket s3 = mf_socket_create(THIRD_SOCKET_FD);
	struct mf_socket_array_node* n3 = mf_socket_array_node_init(s3);
	insert_mf_socket_array(n3,q);
	return 0;


}
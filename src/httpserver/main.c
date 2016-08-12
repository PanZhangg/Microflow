#include "cgi.h"

int main(){
	char buf[32];
	wr_init();
	while(scanf("%s" ,buf) >0){
		if(strcmp("quit" ,buf) == 0)
			break;
	}

	wr_uninit();
}


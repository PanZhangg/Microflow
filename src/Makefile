TARGET = microflow
MF_SOURCE = main.c ../src/mf_socket.c ../src/mf_logger.c \
	../src/mf_timer.c ../src/mf_switch.c ../src/mf_msg_handler.c ../src/mf_ofmsg_constructor.c \
	../src/mf_devicemgr.c ../src/mf_rx_queue.c ../src/mf_msg_parser.c ../src/mf_mempool.c \
	../src/mf_utilities.c ../src/mf_topomgr.c \
	../src/mf_controller.c ../src/mf_lf_list.c\

HTTP_SOURCE = ./httpserver/wrlib/wrmpool.c ./httpserver/wrlib/wrhash.c ./httpserver/wrlib/wrstring.c ./httpserver/wrlib/misc.c ./httpserver/wrlib/wrio.c \
./httpserver/wrlib/http.c ./httpserver/wrlib/request.c ./httpserver/wrlib/analysis.c ./httpserver/wrlib/mg_compat.c \
./httpserver/cgi.c \

HTTP_OBJECTS = analysis.o http.o cgi.o mg_compat.o misc.o request.o wrhash.o wrio.o wrmpool.o wrstring.o 

LIBS = httpserver.a

$(TARGET): $(MF_SOURCE) $(LIBS) 
	cc -Wall -O3 -W -Wno-unused -Wno-sign-compare -g -D_POSIX_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -D__USE_GNU -o $@ $^ -ldl -lpthread 
	rm $(HTTP_OBJECTS)
httpserver.a : $(HTTP_OBJECTS)
	ar crv httpserver.a $(HTTP_OBJECTS)
$(HTTP_OBJECTS) : $(HTTP_SOURCE)
	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE $^ -ldl -lpthread  
#analysis.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/analysis.c -ldl -lpthread  
#http.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/http.c -ldl -lpthread  
#cgi.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/cgi.c -ldl -lpthread  
#mg_compat.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/mg_compat.c -ldl -lpthread  
#misc.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/misc.c -ldl -lpthread  
#request.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/request.c -ldl -lpthread  
#wrhash.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/wrhash.c -ldl -lpthread  
#wrio.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/wrio.c -ldl -lpthread  
#wrmpool.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/wrmpool.c -ldl -lpthread  
#wrstring.o:
#	gcc -w -c -D_POSIX_SOURCE -D_BSD_SOURCE ./httpserver/wrlib/wrstring.c -ldl -lpthread  
clean:
	rm $(TARGET) $(LIBS) 

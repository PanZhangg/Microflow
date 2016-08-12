microflow: main.c ../src/mf_socket.c ../src/mf_logger.c \
	../src/mf_timer.c ../src/mf_switch.c ../src/mf_msg_handler.c ../src/mf_ofmsg_constructor.c \
	../src/mf_devicemgr.c ../src/mf_rx_queue.c ../src/mf_msg_parser.c ../src/mf_mempool.c \
	../src/mf_utilities.c ../src/mf_topomgr.c \
	../src/mf_controller.c \
	./httpserver/wrlib/wrmpool.c ./httpserver/wrlib/wrhash.c ./httpserver/wrlib/wrstring.c ./httpserver/wrlib/misc.c ./httpserver/wrlib/wrio.c \
	./httpserver/wrlib/http.c ./httpserver/wrlib/request.c ./httpserver/wrlib/analysis.c ./httpserver/wrlib/mg_compat.c \
	./httpserver/cgi.c

	cc -w -g -D_POSIX_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -D__USE_GNU -o $@ $^ -ldl -lpthread 

clean:
	rm microflow 

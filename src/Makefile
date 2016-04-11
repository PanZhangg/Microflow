microflow: main.c ../src/mf_socket.c ../src/mf_logger.c \
			../src/mf_timer.c ../src/mf_switch.c ../src/mf_msg_handler.c ../src/mf_ofmsg_constructor.c \
			../src/mf_devicemgr.c ../src/mf_rx_queue.c ../src/mf_msg_parser.c ../src/mf_mempool.c \
			../src/mf_utilities.c ../src/mf_topomgr.c
	cc -g -Wall -D_GUN_SOURCE -D__USE_GUN -o $@ $^ -pthread

clean:
	rm microflow

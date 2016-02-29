main.out: socket_test.c ../src/mf_socket.c ../src/mf_socket_array.c ../src/mf_rx_queue.c ../src/mf_msg_parser.c ../src/mf_msg_handler.c ../src/mf_ofmsg_constructor.c ../src/mf_logger.c \
			../src/mf_timer.c
	cc -g -Wall -o $@ $^ -pthread

clean:
	rm main.out

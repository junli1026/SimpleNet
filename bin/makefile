CC=g++
SRC=../src2

target:\
	buffer.o poller.o channel.o message.o tcp_server.o\
	rr_server.o rr_client.o publisher.o \
	TestRRClient TestRRServer TestPublisher TestSubscriber

buffer.o:
	$(CC) $(SRC)/buffer.cpp -c -std=c++11 
poller.o:
	$(CC) $(SRC)/poller.cpp -c -std=c++11
message.o:
	$(CC) $(SRC)/message.cpp -c -std=c++11
channel.o:
	$(CC) $(SRC)/channel.cpp -c -std=c++11
tcp_server.o:
	$(CC) $(SRC)/tcp_server.cpp -c -std=c++11
	
rr_client.o:
	$(CC) $(SRC)/request_response/rr_client.cpp -c -std=c++11
rr_server.o:
	$(CC) $(SRC)/request_response/rr_server.cpp -c -std=c++11

publisher.o:
	$(CC) $(SRC)/publish_subscribe/publisher.cpp -c -std=c++11



TestPublisher:
	$(CC) $(SRC)/tests/test_publisher.cpp -o TestPublisher publisher.o tcp_server.o poller.o buffer.o message.o channel.o -lpthread -std=c++11 
#TestSubscriber:
#	$(CC) $(SRC)/tests/test_subscriber.cpp -o TestSubscriber tcp_client.o message.o -std=c++11

TestRRClient:
	$(CC) $(SRC)/tests/test_rr_client.cpp -o TestRRClient rr_client.o message.o buffer.o -std=c++11
TestRRServer:
	$(CC) $(SRC)/tests/test_rr_server.cpp -o TestRRClient rr_server.o tcp_server.o poller.o buffer.o message.o -std=c++11

clean:
	rm -rf *.o
	rm -rf TestRRClient
	rm -rf TestRRServer
	rm -rf TestPublisher
	rm -rf TestSubscriber

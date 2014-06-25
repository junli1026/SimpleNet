CC=g++
SRC=../src
TESTS=../tests

target:\
	buffer.o poller.o channel.o block.o tcp_server.o iohandler.o acceptor.o\
	rr_server.o rr_client.o \
	publisher.o subscriber.o \
	TestRRClient TestRRServer TestPublisher TestSubscriber

buffer.o:
	$(CC) $(SRC)/buffer.cpp -c -std=c++11 
poller.o:
	$(CC) $(SRC)/poller.cpp -c -std=c++11
block.o:
	$(CC) $(SRC)/block.cpp -c -std=c++11
channel.o:
	$(CC) $(SRC)/channel.cpp -c -std=c++11
tcp_server.o:
	$(CC) $(SRC)/tcp_server.cpp -c -std=c++11
iohandler.o:
	$(CC) $(SRC)/iohandler.cpp -c -std=c++11
acceptor.o:
	$(CC) $(SRC)/acceptor.cpp -c -std=c++11

rr_client.o:
	$(CC) $(SRC)/request_response/rr_client.cpp -c -std=c++11
rr_server.o:
	$(CC) $(SRC)/request_response/rr_server.cpp -c -std=c++11

publisher.o:
	$(CC) $(SRC)/publish_subscribe/publisher.cpp -c -std=c++11
subscriber.o:
	$(CC) $(SRC)/publish_subscribe/subscriber.cpp -c -std=c++11


TestPublisher:
	$(CC) $(TESTS)/test_publisher.cpp -o TestPublisher publisher.o tcp_server.o poller.o buffer.o block.o channel.o acceptor.o iohandler.o -lpthread -std=c++11 
TestSubscriber:
	$(CC) $(TESTS)/test_subscriber.cpp -o TestSubscriber subscriber.o block.o buffer.o -std=c++11

TestRRClient:
	$(CC) $(TESTS)/test_rr_client.cpp -o TestRRClient rr_client.o block.o buffer.o -std=c++11
TestRRServer:
	$(CC) $(TESTS)/test_rr_server.cpp -o TestRRServer rr_server.o tcp_server.o poller.o buffer.o block.o  acceptor.o iohandler.o -std=c++11

clean:
	rm -rf *.o
	rm -rf TestRRClient
	rm -rf TestRRServer
	rm -rf TestPublisher
	rm -rf TestSubscriber


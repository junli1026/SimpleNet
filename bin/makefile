CC=g++
SRC=../src2

target: buffer.o poller.o tcp_server.o tcp_client.o

buffer.o:
	$(CC) $(SRC)/buffer.cpp -c -std=c++11 
poller.o:
	$(CC) $(SRC)/poller.cpp -c -std=c++11

tcp_server.o:
	$(CC) $(SRC)/tcp_server.cpp -c -std=c++11

tcp_client.o:
	$(CC) $(SRC)/tcp_client.cpp -c -std=c++11
clean:
	rm -rf *.o

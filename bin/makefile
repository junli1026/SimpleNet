CC=g++
SRC=../src2

target: buffer.o poller.o tcp_server.o tcp_client.o rr_server.o client_main server_main

buffer.o:
	$(CC) $(SRC)/buffer.cpp -c -std=c++11 
poller.o:
	$(CC) $(SRC)/poller.cpp -c -std=c++11

tcp_server.o:
	$(CC) $(SRC)/tcp_server.cpp -c -std=c++11

tcp_client.o:
	$(CC) $(SRC)/tcp_client.cpp -c -std=c++11


rr_server.o:
	$(CC) $(SRC)/request_response/rr_server.cpp -c -std=c++11
client_main:
	$(CC) $(SRC)/request_response/client_main.cpp -o client_main tcp_client.o -std=c++11
server_main:
	$(CC) $(SRC)/request_response/server_main.cpp -o server_main rr_server.o tcp_server.o poller.o buffer.o -std=c++11

clean:
	rm -rf *.o
	rm client_main
	rm server_main

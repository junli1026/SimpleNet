#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include "tcp_server.h"
#include "listen.cpp"

namespace simple{
TcpServer::TcpServer(){
}

TcpServer::~TcpServer(){
}	

void TcpServer::monitor(const char* ip, int port){
	int listenfd = doListen(ip, port, 1024);
	if(listenfd < 0){
		printf("error\n");
		return;
	}else{
		this->acceptor_.add(listenfd);
		this->poller_.addRead(listenfd);
	}
}

void TcpServer::run(){
	this->poller_.poll(200);
	struct epoll_event* ev = NULL;
	Socket *s = NULL;
	int fd;
	while(this->poller_.hasEvent()){ 
		//try
		ev = this->poller_.nextEvent();
		fd = ev->data.fd;
		if(acceptor_.contains(fd)){
			int newfd = this->acceptor_.doAccept(fd);
			acceptContinuous(newfd);
		}else if(ev->events & EPOLLIN){
			this->iohandler_.doRead(fd);
			readContinuous(fd);
		}else{
			this->iohandler_.doWrite(fd);
			writeContinuous(fd);
		}
		//catch
	}
}

}
#include <iostream>

#include "tcp_server.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

namespace simple{
TcpServer::TcpServer(){
}

TcpServer::~TcpServer(){

}	

// return listen fd
int TcpServer::doListen(const char *host, int port, int backlog){
    uint32_t internet_addr; 
    if (host[0]) {
        internet_addr = inet_addr(host);
    }else{
        internet_addr = INADDR_ANY; //default: listen to all IPs on this machine
    }
    //set socket address
    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(struct sockaddr_in));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port);
    socket_addr.sin_addr.s_addr = internet_addr;

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0); //tcp
    if (listen_fd < 0) {
        return -1;
    }

    //set SO_REUSEADDR, so that our server process can quickly reboot, even if socket is still in TIME_WAIT status
    int reuse = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(int)) == -1) {
        return -1;
    }

    if(bind(listen_fd, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr)) == -1) {
        return -1;
    }
    if(listen(listen_fd, backlog) == -1) {
        return -1;
    }
    return listen_fd;
}

bool TcpServer::isAcceptSocket(int fd){
	auto it = this->acceptSockets_.find(fd);
	return (it != this->acceptSockets_.end());
}

void TcpServer::doAccept(ASocket* s){
	const Context& ctx = this->acceptor_.doAccept(s);
	if(ctx.errnumber){
		perror("accept");
		return;
	}
	acceptContinuous(ctx);
}

void TcpServer::doRead(IOSocket* s){
	const Context& ctx = this->iohandler_.doRead(s);
	if(ctx.errnumber){
		perror("read");
		return;
	}
	this->readContinuous(ctx, s);
}

void TcpServer::doWrite(IOSocket* s){
	const Context& ctx = this->iohandler_.doWrite(s);
	if(ctx.errnumber){
		perror("write");
		return;
	}
	this->writeContinuous(ctx, s);
}

void TcpServer::monitor(const char* ip, int port){
	int listenfd = this->doListen(ip, port, 1024);
	if(listenfd < 0){
		printf("error\n");
		return;
	}else{
		std::shared_ptr<ASocket> s = std::make_shared<ASocket>(listenfd);
		s->setNonBlock();
		this->acceptSockets_.insert(std::pair<int, std::shared_ptr<ASocket>>(listenfd, s));
		this->poller_.addRead(s->getFd());
	}
}

void TcpServer::run(){
	this->poller_.poll(200);
	struct epoll_event* ev = NULL;
	Socket *s = NULL;
	int fd;
	while(this->poller_.hasEvent()){ 
		ev = this->poller_.nextEvent();
		fd = ev->data.fd;
		if(isAcceptSocket(fd)){
			this->doAccept(acceptSockets_.find(fd)->second.get());
		}else if(ev->events & EPOLLIN){
			this->doRead(ioSockets_.find(fd)->second.get());
		}else{
			this->doWrite(ioSockets_.find(fd)->second.get());
		}
	}
}

}
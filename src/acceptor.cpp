#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h> 
#include "acceptor.h"

namespace simple{

Acceptor::Acceptor(){
}

Acceptor::~Acceptor(){
}

bool Acceptor::contains(int fd){
	auto it = this->fds_.find(fd);
	return (it != this->fds_.end());
}

void Acceptor::removeFd(int fd){
	this->fds_.erase(fd);
}

void Acceptor::addFd(int fd){
	this->fds_.insert(fd);
}

std::shared_ptr<Connection> Acceptor::doAccept(int fd){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int connfd = accept(fd, (struct sockaddr*)&client_addr, &len);
	if(connfd < 0){
		//to do: throw exception
		assert(0);
	}
	return std::make_shared<Connection>(connfd, false, true);//not client, and set nonblock
}

}
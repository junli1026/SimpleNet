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

void Acceptor::erase(int fd){
	this->fds_.erase(fd);
}

void Acceptor::add(int fd){
	if(this->contains(fd)){
		//to do:: throw exception
	}else{
		this->fds_.insert(fd);
	}
}

std::shared_ptr<Connection> Acceptor::doAccept(int fd){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int connfd = accept(fd, (struct sockaddr*)&client_addr, &len);
	//to do: check errors
	return std::make_shared<Connection>(connfd, false, true);//not client, and set nonblock
}

}
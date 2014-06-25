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
	auto it = this->sockets_.find(fd);
	return (it != this->sockets_.end());
}

void Acceptor::erase(int fd){
	this->sockets_.erase(fd);
}

void Acceptor::add(int fd){
	if(this->contains(fd)){
		//to do:: throw exception
	}else{
		auto s = std::make_shared<ASocket>(fd);
		s->setNonBlock();
		this->sockets_.insert(std::pair<int, std::shared_ptr<ASocket>>(fd, s));
	}
}

int Acceptor::doAccept(int fd){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int connfd = accept(fd, (struct sockaddr*)&client_addr, &len);
	return connfd;
}

}
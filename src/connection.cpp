#include "connection.h"
#include <unistd.h>
#include <fcntl.h>

namespace simple{

Connection::Connection(int fd, bool isClient, bool nonblock){
	this->rbufptr_ = std::make_shared<Buffer>();
	this->wbufptr_ = std::make_shared<Buffer>();
	this->isClient_ = isClient;
	this->status_ = ConnAlive;
	this->fd_ = fd;
	if(nonblock){
	    int flag = fcntl(this->fd_, F_GETFL, 0);
	    if (-1 == flag){
	    	//to do: throw exception
	    	return;
	    }
	    fcntl(this->fd_, F_SETFL, flag | O_NONBLOCK);
	}
	if(this->fd_ < 0){
		this->status_ = ConnError;
	}
}

Connection::~Connection(){
	this->doClose();
}

std::shared_ptr<Buffer> Connection::readBuffer(){
	return this->rbufptr_;
}

std::shared_ptr<Buffer> Connection::writeBuffer(){
	return this->wbufptr_;
}

ConnectionStatus Connection::status(){
	return this->status_;
}

void Connection::setClient(){
	this->isClient_ = true;
}

void Connection::setServer(){
	this->isClient_ = false;
}

bool Connection::isClient(){
	return this->isClient_;
}

bool Connection::isServer(){
	return !(this->isClient_);
}

int Connection::fd(){
	return this->fd_;
}

void Connection::doClose(){
	if(this->status_ != ConnClosed){		
		close(this->fd_);
		this->rbufptr_->clear();
		this->wbufptr_->clear();
		this->status_ = ConnClosed;
	}
}

}
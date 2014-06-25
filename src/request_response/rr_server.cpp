#include <memory>
#include <iostream>
#include <string.h>
#include <assert.h>
#include "rr_server.h"

namespace simple{

RRServer::RRServer(){
	this->response_ = nullptr;
}

RRServer::~RRServer(){}

void RRServer::acceptContinuous(int newfd){
	this->iohandler_.add(newfd);
	this->poller_.addRead(newfd);
}

void RRServer::readContinuous(int fd){
	auto rbuf = this->iohandler_.getReadBuffer(fd);
	auto wbuf = this->iohandler_.getWriteBuffer(fd);
	std::shared_ptr<Block> b = rbuf->retrieveBy("\r\n", 2);
	if(b == nullptr){
		return;
	}else{ // got an complete package, ending with "\r\n"	
		if(this->response_ != nullptr){
			Block resp = this->response_(b);
			wbuf->append(resp.begin(), resp.size());
			wbuf->append("\r\n", 2);
		}
		this->poller_.mod2Write(fd);
	}

}

void RRServer::writeContinuous(int fd){
	auto wbuf = this->iohandler_.getWriteBuffer(fd);
	if(wbuf->size() == 0){ //all data has been write to system buffer
		this->poller_.mod2Read(fd);
	}
}

void RRServer::onRequest(std::function<Block(std::shared_ptr<Block>)> cb){
	this->response_ = cb;
}

void RRServer::loop(){
	while(true){
		this->run();
	}
}

}

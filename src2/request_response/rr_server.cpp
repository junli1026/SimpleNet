#include <memory>
#include <iostream>
#include <string.h>
#include "rr_server.h"

namespace simple{

RRServer::RRServer(){}

RRServer::~RRServer(){}

void RRServer::acceptContinuous(int newfd){
	this->iohandler_.add(newfd);
	this->poller_.addRead(newfd);
}

void RRServer::readContinuous(int fd){
	auto rbuf = this->iohandler_.getReadBuffer(fd);
	auto wbuf = this->iohandler_.getWriteBuffer(fd);
	std::vector<uint8_t> data = rbuf->retrieveBy("\r\n", 2);
	if(data.size() == 0){
		return;
	}else{ // got an complete package, ending with "\r\n"		
		if(this->response_ != nullptr){
			Message r = this->response_(Message(data));
			wbuf->append(r.begin(), r.size());
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

void RRServer::onRequest(std::function<Message(Message)> cb){
	this->response_ = cb;
}

void RRServer::loop(){
	while(true){
		this->run();
	}
}

}

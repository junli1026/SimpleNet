#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <assert.h>
#include "../tcp_server.h"
#include "../channel.h"
#include "../buffer.h"
#include "publisher.h"

namespace simple{

const Message CMDPublish("CMDPublish");
const Message CMDClose("CMDClose");

Publisher::Publisher(const char* ip, int port){
	this->ip_ = std::string(ip, strlen(ip));
	this->port_ = port;
	this->monitor(ip, port);
}

Publisher::~Publisher(){}

void Publisher::acceptContinuous(int newfd){
	this->iohandler_.add(newfd);
	this->poller_.addWrite(newfd);
}

void Publisher::readContinuous(int fd){}

void Publisher::writeContinuous(int fd){}

void Publisher::sendCommand(const Message& m){
	this->channel_.writeMessage(m);
}

void run(void* data){
	//Publisher* p = static_cast<Publisher*>(data);
	//p->loop();
}

void Publisher::runService(){
	//pthread_create(&this->serviceId_, NULL, run, this);
}

void Publisher::stopService(){
	this->sendCommand(CMDClose);
}

void Publisher::publish(const void* src, size_t sz){
	if(src == NULL || sz == 0){
		return;
	}
	this->mq_.push(Message(src, sz));
	sendCommand(CMDPublish);
}

void Publisher::loop(){
	while(true){
		if(this->channel_.hasMessage()){
			Message cmd = this->channel_.getMessage();
			if(cmd == CMDPublish){
				assert(mq_.size() > 0);
				Message msg = mq_.front();
				mq_.pop();			 
				for(auto m : this->iohandler_.getSocketMap()){
					m.second->writeBuffer()->append(msg);
				}
			}else{ // must to close or error happened
				break;
			}
		}
		this->run();
	}
}

}

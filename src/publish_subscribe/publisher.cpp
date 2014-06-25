#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <assert.h>
#include "../tcp_server.h"
#include "../channel.h"
#include "../buffer.h"
#include "../block.h"
#include "publisher.h"

namespace simple{

const std::string CMDClose("CMDClose");
const std::string CMDPublish("CMDPublish");

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

void Publisher::sendCommand(const std::string& cmd){
	this->channel_.writeMessage(cmd);
}

void* runThread(void* data){
	Publisher* p = static_cast<Publisher*>(data);
	p->loop();
}

void Publisher::runService(){
	pthread_create(&this->serviceId_, NULL, &runThread, this);
}

void Publisher::stopService(){
	this->sendCommand(CMDClose);
}

void Publisher::publish(const void* src, size_t sz){
	if(src == NULL || sz == 0){
		return;
	}
	std::shared_ptr<Block> b = std::make_shared<Block>(src, sz);
	this->mq_.push(b);
	sendCommand(CMDPublish);
}

void Publisher::loop(){
	while(true){
		if(this->channel_.hasMessage()){			
			std::string cmd = this->channel_.getMessage();
			if(cmd == CMDPublish){
				assert(mq_.size() > 0);
				std::shared_ptr<Block> msg = mq_.front();
				mq_.pop();
				for(auto m : this->iohandler_.getSocketMap()){
					m.second->writeBuffer()->append(msg->begin(), msg->size());
					m.second->writeBuffer()->append("\r\n", 2);
				}
			}else{ // must to close or error happened
				break;
			}	
		}
		this->run();
	}
}

}

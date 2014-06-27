#include "eventloop.h"
#include <functional>
#include <string>
#include <assert.h>

namespace simple{

void EventLoop::registerCmdhandler(std::function<void(std::string)> h){
	this->cmdhandler_ = h;
}

void EventLoop::handleCommand(){
	std::string cmd;
	while(this->channelptr_->hasMessage()){
		cmd = this->channelptr_->getMessage();
		this->cmdhandler_(cmd);
	}
}

void EventLoop::handleSocket(){
	this->poller_.poll(200);
	struct epoll_event* ev = NULL;
	int fd;
	while(this->poller_.hasEvent()){
		ev = this->poller_.nextEvent();
		fd = ev->data.fd;
		if(acceptor_.contains(fd)){
			acceptContinuous(this->acceptor_.doAccept(fd););
		}else if(ev->events & EPOLLIN){
			auto conn = this->connMap_.find(fd);
			
			this->iohandler_.doRead(fd);
			readContinuous(fd);
		}else{
			this->iohandler_.doWrite(fd);
			writeContinuous(fd);
		}
		//catch
	}
}

void EventLoop::loop(){
	while(true){
		if(acceptCmd_){
			assert(this->cmdhandler_ != nullptr);
		}
		handleSocket();
	}
}

}
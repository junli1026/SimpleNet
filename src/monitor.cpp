#include "monitor.h"
#include <sys/epoll.h>
#include <assert.h>

namespace simple{

Monitor::Monitor(){
}

Monitor::~Monitor(){
}

void Monitor::monitorAccept(int fd){
	if(fd < 0){
		//error
		return;
	}
	this->acceptFds_.insert(fd);
	this->poller_.addRead(fd);
}

void Monitor::monitorRead(std::shared_ptr<Connection> cptr){
	if(cptr == nullptr){
		return;
	}
	auto st = cptr->status();
	if(st == ConnAlive){ 
		this->poller_.addRead(cptr->fd());
		cptr->setStatus(ConnRead);
	}else if(st == ConnWrite){
		this->poller_.mod2Write(cptr->fd());
		cptr->setStatus(ConnWrite);
	}else{
		assert(st == ConnRead);
	}
}

void Monitor::monitorWrite(std::shared_ptr<Connection> cptr){
	if(cptr == nullptr){
		return;
	}
	auto st = cptr->status();
	if(st == ConnAlive){
		this->poller_.addWrite(cptr->fd());
	}else if(st == ConnRead){
		this->poller_.mod2Write(cptr->fd());
	}else{
		assert(st == ConnWrite);
	}
}

void Monitor::stopMonitor(std::shared_ptr<Connection> cptr){
	if(cptr == nullptr){
		return;
	}
	auto st = cptr->status();
	assert(st == ConnRead || st == ConnWrite);
	this->poller_.remove(cptr->fd());
}

bool Monitor::hasEvent(){
	if(this->poller_.hasEvent()){
		return true;
	}else{
		this->poller_.poll(200);
		return this->poller_.hasEvent();
	}
}

bool Monitor::isAcceptFd(int fd){
	auto it = this->acceptFds_.find(fd);
	return (it != this->acceptFds_.end());
}

Event<int> Monitor::nextEvent(){
	if(this->hasEvent()){
		struct epoll_event* ev = this->poller_.nextEvent();
		assert(ev);
		int fd = ev->data.fd;
		if(this->isAcceptFd(fd)){
			return Event<int>(GL_EventAccept, fd);
		}else if(ev->events & EPOLLIN){
			return Event<int>(GL_EventRead, fd);
		}else{
			assert(ev->events & EPOLLOUT);
			return Event<int>(GL_EventRead, fd);
		}
	}else{
		return Event<int>(GL_EventNone, -1);
	}
}

}

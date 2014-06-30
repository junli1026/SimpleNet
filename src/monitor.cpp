#include "monitor.h"

namespace simple{

Monitor::Monitor(std::shared_ptr<Poller> p, 
				std::shared_ptr<Channel> c
				std::shared_ptr<ConnectionMap> m){
	this->poller_ = p;
	this->channel_ = c;
	this->connMap_ = m;
}

~Monitor(){
}

void Monitor::monitorAccept(int fd){
	if(fd < 0){
		//error
		return;
	}
	this->acceptFds_.insert(fd);
	this->poller_->addRead(fd);
}

void Monitor::monitorRead(std::shared_ptr<Connection> cptr){
	if(cptr == nullptr){
		return;
	}
	auto st = cptr->status();
	if(st == ConnAlive){ 
		this->poller_->addRead(cptr->fd());
		cptr->setStatus(ConnRead);
	}else if(st == ConnWrite){
		this->poller_->mod2Write(cptr->fd());
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
		this->poller_->addWrite(cptr->fd());
	}else if(st == ConnRead){
		this->poller_->mod2Write(cptr->fd());
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
	this->poller_->remove(cptr->fd());
}




}
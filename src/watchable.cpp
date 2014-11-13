#include "watchable.h"

namespace simple{

Watchable::Watchable(){
	init();
}

Watchable::Watchable(int fd){
	init();
	this->fd_ = fd; 
}

Watchable::Watchable(int fd, std::shared_ptr<Monitor> m){
	init();
	this->fd_ = fd;
	this->monitor_ = m;
}

void Watchable::setFd(int fd){
	this->fd_ = fd;
}

void Watchable::setMonitor(std::shared_ptr<Monitor> m) {
	this->monitor_ = m;
	if(this->readEnabled) {
		this->monitor_->monitorRead(this);
	}

	if(this->writeEnabled) {
		this->monitor_->monitorWrite(this);
	}
}

void Watchable::enableRead() {
	if(this->readEnabled) return;

	this->readEnabled = true;
	this->monitor_->monitorRead(this); //if this->monitor == nullptr, let it fail
}

void Watchable::disableRead(){
	if(!this->readEnabled) return;

	this->readEnabled = false;
	this->monitor_->stopMonitor(this);
}

void Watchable::enableWrite(){
	if(this->writeEnabled) return;

	this->writeEnabled = true;
	this->monitor_->monitorWrite(this);
}

void Watchable::disableRead(){
	if(!this->readEnabled) return;

	this->readEnabled = false;
	this->monitor_->stopMonitor(this);
}

void Watchable::getFd(){
	return this->fd_;
}


}
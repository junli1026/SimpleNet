#include "poller.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

namespace simple{

Poller::Poller(){
	this->ev_index_ = 0;
	this->ev_count_ = 0;
	this->epfd_ = epoll_create(1024);
}

Poller::~Poller(){
}

void Poller::poll(int timeout){
	this->ev_count_ = epoll_wait(this->epfd_, this->events_, DefaultEventsSize, timeout); //timeout
	this->ev_index_ = 0;
}

bool Poller::hasEvent(){
	if(this->ev_count_ > 0 && this->ev_index_ < this->ev_count_){
		return true;
	}else{
		return false;
	}
}

struct epoll_event* Poller::nextEvent(){
	if(this->hasEvent()){
		auto ret = &this->events_[this->ev_index_];
		this->ev_index_ ++;
		return ret;
	}else{
		return NULL;
	}
}

inline void setEpollEvent(struct epoll_event& ev, int fd, bool enableRead, bool enableWrite){
	memset(&ev, 0, sizeof(struct epoll_event));
	if(enableRead){
		ev.events |= EPOLLIN;
	}
	if(enableWrite){
		ev.events |= EPOLLOUT;
	}
	ev.data.fd = fd;
}

void Poller::addRead(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, true, false);
	int error = epoll_ctl(this->epfd_, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("addRead");
	}
}

void Poller::addWrite(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, false, true);
	int error = epoll_ctl(this->epfd_, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("addWrite");
	}
}

void Poller::addReadWrite(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, true, true);
	int error = epoll_ctl(this->epfd_, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("addWrite");
	}
}

void Poller::remove(int fd){
	epoll_ctl(this->epfd_, EPOLL_CTL_DEL, fd, NULL);
}

void Poller::mod2Read(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, true, false);
	if(epoll_ctl(this->epfd_, EPOLL_CTL_MOD, fd, &ev) == -1){
        printf("epoll mod fails\n");
    }
}

void Poller::mod2Write(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, false, true);
	if(epoll_ctl(this->epfd_, EPOLL_CTL_MOD, fd, &ev) == -1){
        printf("epoll mod fails\n");
    }
}

void Poller::mod2ReadWrite(int fd){
	struct epoll_event ev;
	setEpollEvent(ev, fd, true, true);
	if(epoll_ctl(this->epfd_, EPOLL_CTL_MOD, fd, &ev) == -1){
        printf("epoll mod fails\n");
    }
}

}
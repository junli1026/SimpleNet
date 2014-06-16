#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <assert.h>
#include "channel.h"

using namespace std;

namespace simple{

int set_fd_nonblocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if (-1 == flag) {
        return -1;
    }
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    return 0;
}

inline int watch_read(int epfd, int fd){
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int error = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("watch_read");
	}
	return error;
}

inline int watch_write(int epfd, int fd){
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	int error = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("watch_write");
	}
	return error;
}

Channel::Channel(){
	int fd[2];
	if(pipe(fd)){
		this->healthy_ = false;
		return;
	}

	this->rfd_ = fd[0];
	this->wfd_ = fd[1];
    if(set_fd_nonblocking(this->rfd_) || set_fd_nonblocking(this->wfd_)){
    	this->healthy_ = false;
    	return;
    }
    this->epfd_ = epoll_create(1024);
    if(this->epfd_ < 0){
    	goto _fail;
    }
    if(watch_read(this->epfd_, this->rfd_) || watch_write(this->epfd_, this->wfd_)){
    	goto _fail;
    }
	this->healthy_ = true;
	return;

_fail:
	close(this->rfd_);
	close(this->wfd_);
	this->healthy_ = false;
}

Channel::~Channel(){
	if(this->healthy_){
		close(this->rfd_);
		close(this->wfd_);
	}
}

bool Channel::HasData(){
	struct epoll_event events[1];
	int n = epoll_wait(this->epfd_, events, 1, 0);
	if(n > 0){
		assert(n == 1);
		assert(events[0].data.fd == this->rfd_);
		return true;
	}else{
		return false;
	}
}

bool Channel::Healthy(){
	return this->healthy_;
}

int Channel::Readfd(){
	return this->rfd_;
}

int Channel::Writefd(){
	return this->wfd_;
}

}//namespace simple

int main(){
	simple::Channel c;
	if(c.Healthy()){
		std::cout << "helloworld";
	}
}


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <assert.h>
#include <algorithm>
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
		return;
	}

	this->rfd_ = fd[0];
	this->wfd_ = fd[1];
    if(set_fd_nonblocking(this->rfd_) || set_fd_nonblocking(this->wfd_)){
    	return;
    }
    this->epfd_ = epoll_create(1024);
    if(this->epfd_ < 0){
    	goto _fail;
    }
    if(watch_read(this->epfd_, this->rfd_)) {// || watch_write(this->epfd_, this->wfd_)){
    	goto _fail;
    }
	return;

_fail:
	close(this->rfd_);
	close(this->wfd_);
}

Channel::~Channel(){
	close(this->rfd_);
	close(this->wfd_);
}

bool Channel::hasData(){
	struct epoll_event events[1];
	int n = epoll_wait(this->epfd_, events, 1, 0);
	if(n > 0){
		assert(n == 1);
		assert(events[0].data.fd == this->rfd_);
		uint8_t buf[1024];
		int c = read(this->rfd_, buf, 1024);
		if(c > 0){
			this->buf_.append(buf, c);
			return true;
		}
	}
	return false;	
}

bool Channel::hasMessage(){
	while(hasData()){
	}
	std::vector<uint8_t> data = this->buf_.retrieveBy("\r\n", 2);
	if(data.size() > 0){
		this->q_.push(Message(data));
		return true;
	}else{
		return q_.size() > 0;
	}
}

Message Channel::getMessage(){
	if(hasMessage() || this->q_.size() > 0){
		auto ret = this->q_.front();
		q_.pop();
		return ret;
	}else{
		return Message();
	}
}

void Channel::writeMessage(const Message& m){
	int n = write(this->wfd_, m.begin(), m.size());
	assert(n == m.size());
	n = write(this->wfd_, "\r\n", 2);
	assert(n == 2);
}

}//namespace simple

/*
using namespace simple;
int main(){
	Channel c;

	c.writeMessage(Message("thisis one"));;
	c.writeMessage(Message("this is message two"));
	c.writeMessage(Message("hello world"));

	while(true){
		if(c.hasMessage()){
			auto ret = c.getMessage();
			std::cout << ret.dump2String() << std::endl;
			fflush(stdout);
		}
	}
}
*/


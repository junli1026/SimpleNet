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

bool Channel::hasControlWords(){
	while(hasData()){
	}
	std::vector<uint8_t> ret = this->buf_.retrieveBy("\r\n", 2);
	if(ret.size() > 0){
		ControlWords w;
		auto prev = ret.begin();
		auto curr = std::find(prev, ret.end(), static_cast<uint8_t>('\0'));
		while(curr != ret.end()){
			w.push_back(std::string(prev, curr));
			curr ++;
			prev = curr;
			curr = std::find(prev, ret.end(), static_cast<uint8_t>('\0'));
		}
		this->q_.push(w);
		return true;
	}else{
		return false;
	}
}

ControlWords Channel::getControlWords(){
	if(hasControlWords() || this->q_.size() > 0){
		auto ret = this->q_.front();
		q_.pop();
		return ret;
	}else{
		ControlWords v;
		return v;
	}
}

void Channel::writeControlWords(const ControlWords& ws){
	int n;
	const char* tmp;
	for(auto str: ws){
		tmp = str.c_str();
		n = write(this->wfd_, tmp, strlen(tmp) + 1);
		assert(n == strlen(tmp) + 1);
	}
	n = write(this->wfd_, "\r\n", 2);
	assert(n == 2);
}

}//namespace simple

/*
using namespace simple;
int main(){
	Channel c;

	ControlWords words;
	words.push_back("this is a test");
	words.push_back("hello world");
	words.push_back("junli");

	c.writeControlWords(words);

	if(c.hasControlWords()){
		auto ret = c.getControlWords();
		for(std::string str: ret){
			std::cout << str << std::endl;
			fflush(stdout);
		}
	}

}
*/


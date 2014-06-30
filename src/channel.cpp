#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <assert.h>
#include <algorithm>
#include <memory>
#include "channel.h"

namespace simple{

inline int set_fd_nonblocking(int fd){
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

bool Channel::hasEvent(){
	while(hasData()){
	}
	auto b = this->buf_.retrieveBy("\r\n", 2);
	if(b != nullptr){
		this->qheader_.push(b->dump2String());
		return true;
	}else{
		return qheader_.size() > 0;
	}
}

void Channel::writeHeader(const std::string& header){
	Block b(header);
	int n = write(this->wfd_, b.begin(), b.size());
	assert(n == b.size());
	n = write(this->wfd_, "\r\n", 2);
	assert(n == 2);
}

void Channel::addEvent(const std::string& h){
	this->qbody_.push(nullptr);
	this->writeHeader(h);
}

void Channel::addEvent(const std::string& h, const void* data, size_t sz){
	std::shared_ptr<Block> b = std::make_shared<Block>(data, sz);
	this->qbody_.push(b);
	this->writeHeader(h);
}

void Channel::addEvent(const std::string& h, std::shared_ptr<Block> b){
	this->qbody_.push(b);
	this->writeHeader(h);
}

void Channel::addPublishEvent(std::shared_ptr<Block> b){
	this->addEvent(this->cmdPublish, b);	
}

void Channel::addListenEvent(std::shared_ptr<Block> b){
	this->addEvent(this->cmdListem, b);
}

void Channel::addConnectEvent(std::shared_ptr<Block> b){
	this->addEvent(this->cmdConnect, b);
}

void Channel::addExitEvent(std::shared_ptr<Block> b){
	this->addEvent(this->cmdExit);
}

Event<std::shared_ptr<Block>> Channel::nextEvent(std::string& cmd){
	if(hasEvent()){
		std::string h = this->qheader_.front();
		qheader_.pop();
		std::shared_ptr<Block> b = this->qbody_.front();
		qbody_.pop();
		cmd = h;
		if(h == this->cmdPublish)
			return Event<std::shared_ptr<Block>>(EventPublish, b);
		else if(h == this->cmdExit)
			return Event<std::shared_ptr<Block>>(EventExit, b);
		else if(h == this->cmdConnect)
			return Event<std::shared_ptr<Block>>(EventConnect, b);
		else if(h == this->cmdListen)
			return Event<std::shared_ptr<Block>>(EventListen, b);
	}
	return Event<std::shared_ptr<Block>>(EventNone, nullptr);
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


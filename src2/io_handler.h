#include "socket.h"
#include "context.h"
#include "buffer.h"
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

namespace simple{
class IOHandler{
private:
	Context ctx_;

	const IOHandler& operator=(const IOHandler& IOHandler){}
	IOHandler(const IOHandler& IOHandler){}
	
public:
	IOHandler(){}
	~IOHandler(){}

	const Context& doRead(IOSocket* s){
		int fd = s->getFd();
		auto buf = s->getReadBuffer();
		uint8_t b[1024];
		int n = read(fd, b, 1024);
		if(n < 0){
			std::cout << "n < 0" << std::endl;
			fflush(stdout);

			this->ctx_.errnumber = errno;
		}else if(n == 0){
			std::cout << "n = 0" << std::endl;
			fflush(stdout);

			s->setClose();
			this->ctx_.errnumber = 0;
		}else{
			this->ctx_.errnumber = 0;

			std::cout << "received: ";
			for(int i = 0; i < n; i++){
				std::cout << b[i];
			}
			std::cout << std::endl;
			fflush(stdout);


			buf->append(b, n);
		}
		this->ctx_.operation = SockOpRead;
		this->ctx_.connfd = -1;
		return this->ctx_;
	}

	const Context& doWrite(IOSocket* s){
		
		int fd = s->getFd();
		auto buf = s->getWriteBuffer();
		if(buf->size() == 0){
			this->ctx_.errnumber = 0;
			return this->ctx_;
		}

		size_t sz = 1024;
		if(buf->size() < sz){
			sz = buf->size();
		}
		
		int n = write(fd, buf->begin(), sz);
		std::cout << " after write" << std::endl;
		if(n < 0){
			this->ctx_.errnumber = errno;
		}else if(n == 0){

		}else{
			this->ctx_.errnumber = 0;
			buf->truncate(n);
		}

		std::cout << "write " << n << " bytes" << std::endl;
		fflush(stdout);

		this->ctx_.operation = SockOpWrite;
		this->ctx_.connfd = -1;
		return this->ctx_;
	}

};

class IOHandler{
private:
	std::shared_ptr<Poller> poller_;
	std::map<int, std::shared_ptr<IOSocket>> sockets_;
	
	IOHandler& operator=(const IOHandler& IOHandler){}
	IOHandler(const IOHandler& IOHandler){}
public:
	bool contains(int fd);
	void add(int fd);
	void erase(int fd);
	void doRead(int fd);
	void doWrite(int fd);
	
	IOHandler(std::shared_ptr<Poller> poller);
	~IOHandler();
};
}

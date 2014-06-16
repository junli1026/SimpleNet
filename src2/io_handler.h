#include "socket.h"
#include "context.h"
#include "buffer.h"
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

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
			this->ctx_.errnumber = errno;
		}else if(n == 0){
			s->setClose();
			this->ctx_.errnumber = 0;
		}else{
			this->ctx_.errnumber = 0;
			buf->append(b, n);
		}
		this->ctx_.operation = SockOpRead;
		this->ctx_.connfd = -1;
		return this->ctx_;
	}

	const Context& doWrite(IOSocket* s){
		int fd = s->getFd();
		auto buf = s->getWriteBuffer();
		size_t sz = 1024;
		if(buf->size() < sz){
			sz = buf->size();
		}
		int n = write(fd, buf->begin(), sz);
		if(n < 0){
			this->ctx_.errnumber = errno;
		}else{
			this->ctx_.errnumber = 0;
		}
		this->ctx_.operation = SockOpWrite;
		this->ctx_.connfd = -1;
		return this->ctx_;
	}

};
}
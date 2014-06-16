#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <memory>
#include <fcntl.h>
#include "buffer.h"

namespace simple{
class Socket{
private:
	int fd_;
	int close_;
	const Socket& operator=(const Socket&){}
	Socket(const Socket&){}
public:
	Socket(){
		this->fd_ = -1; 
		this->close_ = false; 
	}

	Socket(int fd){
		this->fd_ = fd;
	}

	int getFd(){
		return this->fd_;
	}

	void setNonblocking(){
    	int flag = fcntl(this->fd_, F_GETFL, 0);
    	if (-1 == flag){
        	return;
    	}
    	fcntl(this->fd_, F_SETFL, flag | O_NONBLOCK);
    }

	bool close(){
		return this->close_;
	}

	void setClose(){
		this->close_ = true;
	}
};

class ASocket: public Socket{
public:
	ASocket():Socket(){}
	ASocket(int fd):Socket(fd){}
};

class IOSocket: public Socket{
private:
	std::shared_ptr<Buffer> wbuf_;
	std::shared_ptr<Buffer> rbuf_;

public:
	IOSocket(): Socket(){
		this->wbuf_ = std::make_shared<Buffer>();
		this->rbuf_ = std::make_shared<Buffer>();
	}

	IOSocket(int fd): Socket(fd){
		this->wbuf_ = std::make_shared<Buffer>();
		this->rbuf_ = std::make_shared<Buffer>();
	}

	Buffer* getWriteBuffer(){
		return this->wbuf_.get();
	}

	Buffer* getReadBuffer(){
		return this->rbuf_.get();
	}
};

}

#endif
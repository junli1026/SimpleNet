#include "subscriber.h"
#include "../buffer.h"

namespace simple{
Subscriber::Subscriber(){}

Subscriber::~Subscriber(){};

void Subscriber::subscribe(const char* host, int port){
	int fd = this->connector_.doConnect(host, port);
	this->fd_ = fd;	
}

std::vector<uint8_t> Subscriber::receiveData(){
	Buffer buf;
	std::vector<uint8_t> ret;
	uint8_t b[1024];
	int n;
	while((n = read(this->fd_, b, 1024)) > 0){
		buf.append(b, n);
		ret = buf.retrieveBy("\r\n", 2);
		if(ret.size() != 0){
			break;
		}
	}
	return ret;
}

}

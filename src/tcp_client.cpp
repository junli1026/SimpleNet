#include "tcp_client.h"
#include "buffer.h"
#include "buffer.h"
#include <iostream>
#include <string.h>

namespace simple{

TcpClient::TcpClient(){

}

TcpClient::~TcpClient(){

}

void TcpClient::doConnect(const char* host, int port){
	int fd = this->connector_.doConnect(host, port);
	this->fd_ = fd;
}

void TcpClient::sendData(const void* src, size_t sz, bool end){
	int n = write(this->fd_, src, sz);
	if(end){
		n = write(this->fd_, "\r\n", 2);
	}
}

std::vector<uint8_t> TcpClient::receiveData(){
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

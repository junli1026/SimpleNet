#include "tcp_client.h"

#include <iostream>

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
	std::vector<uint8_t> ret;
	uint8_t buf[1024];
	int n;
	while((n = read(this->fd_, buf, 1024)) > 0){
		for(int i = 0; i < n; i++){
			std::cout << static_cast<char>(buf[i]);
		}
		std::cout << std::endl;
		ret.reserve(ret.size() + n);
		std::copy(buf, buf+n, ret.end());
	}
	return ret;
}


}

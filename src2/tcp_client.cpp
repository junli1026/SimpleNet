#include "tcp_client.h"

namespace simple{

TcpClient::TcpClient(){

}

TcpClient::~TcpClient(){

}

void TcpClient::doConnect(const char* host, int port){
	int fd = this->connector_.doConnect(host, port);
}

}

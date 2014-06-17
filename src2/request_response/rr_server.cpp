#ifndef _RR_SERVER_H_
#define _RR_SERVER_H_

#include "../tcp_server.h"
#include <memory>
#include <iostream>
#include <string.h>

namespace simple{
class RRServer : public TcpServer 
{
public:
	RRServer(){}
	~RRServer(){}

	void acceptContinuous(const Context& ctx) override{
		int newfd = ctx.connfd;
		std::shared_ptr<IOSocket> s = std::make_shared<IOSocket>(newfd);
		s->setNonBlock();
		auto ret = this->ioSockets_.insert(std::pair<int, std::shared_ptr<IOSocket>> (newfd, s));
		if(ret.second == false){
			std::cout << newfd << "already exists" << std::endl;
			return;
		}
		this->poller_.addRead(newfd);
	}

	void readContinuous(const Context& ctx, IOSocket* s) override{
		auto buf = s->getReadBuffer();
		std::vector<uint8_t> data = buf->retrieveBy("\r\n", 2);
		if(data.size() == 0){
			return;
		}else{ // got an complete package, ending with "\r\n"		
			for(auto a: data){
				std::cout << a;
			}
			std::cout << std::endl;
			this->poller_.mod2Write(s->getFd());

			const char* response = "this is a response\r\n";
			s->getWriteBuffer()->append(response, strlen(response));
		}
	}

	virtual void writeContinuous(const Context& ctx, IOSocket* s) override{
		auto buf = s->getWriteBuffer();
		if(buf->size() == 0){ //all data has been write to system buffer
			this->poller_.mod2Read(s->getFd());
		}
	}
};
}
#endif
	#ifndef _PUBLISHER_H_
#define _PUBLISHER_H_

#include "../tcp_server.h"
#include "../channel.h"

#include <vector>
namespace std simple{

class Publisher: public TcpServer {
private:
	typedef std::vector<uint8_t> Message;
	typedef SafeQueue<Message> MessageQueue;

	MessageQueue mq_; //not necessary to be a thread-safe queue; add-> push + write channel
	Channel channel_;
	
public:
	RRServer(){}
	~RRServer(){}

	void publish(const void* src, size_t sz){

	}

	void run(){
		//new thread begin loop
	}

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

	//the only situation that a port is readable is that HEARTBEAT comes
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
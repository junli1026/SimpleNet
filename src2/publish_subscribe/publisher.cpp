#ifndef _PUBLISHER_H_
#define _PUBLISHER_H_

#include "../tcp_server.h"
#include "../channel.h"
#include "../buffer.h"
#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <assert.h>

namespace simple{

const std::string CMDPublish("CMDPublish");
const std::string CMDClose("CMDClose"); 

class Publisher: public TcpServer {
private:
	typedef std::vector<uint8_t> Message;
	typedef std::queue<Message> MessageQueue;

	MessageQueue mq_; //not necessary to be a thread-safe queue; add-> push + write channel
	Channel channel_;
	
	void sendControlWords(ControlWords w){
		this->channel_.writeControlWords(w);
	}

	void handleCommand(ControlWords w){
		if(w.size() <= 0){
			return;
		}
		if(w[0] == CMDPublish){
			assert(mq_.size() > 0);
			Message msg = mq_.front();
			mq_.pop();

			std::cout << "get Message from queue:";
			for(auto c : msg){
				std::cout << static_cast<char>(c);
			}
			std::cout << std::endl;
			fflush(stdout);

			 
			for(auto m : this->ioSockets_){
				std::cout << "before append : " << m.second->getWriteBuffer()->size() << std::endl;
				m.second->getWriteBuffer()->append(msg);
				std::cout << "after append : " << m.second->getWriteBuffer()->size() << std::endl; 
				fflush(stdout);
			}

		}else if(w[0] == CMDClose){

		}else{

		}
	}

public:
	Publisher(){}
	~Publisher(){}

	void publish(const void* src, size_t sz){
		const uint8_t* 	s = static_cast<const uint8_t*>(src);
		Message m(s, s + sz);
		this->mq_.push(m);
		ControlWords w;
		w.push_back(CMDPublish);
		sendControlWords(w);
	}

	void runThreading(){
		while(true){
			if(this->channel_.hasControlWords()){
				ControlWords w = this->channel_.getControlWords();
				handleCommand(w);
			}
			this->run();
		}
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
		this->poller_.addWrite(newfd);
		std::cout << "a new connection coming " << std::endl;
	}

	//the only situation that a port is readable is that HEARTBEAT comes
	void readContinuous(const Context& ctx, IOSocket* s) override{
		/*
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
		*/
	}

	virtual void writeContinuous(const Context& ctx, IOSocket* s) override{
		/*
		auto buf = s->getWriteBuffer();
		if(buf->size() == 0){ //all data has been write to system buffer
			this->poller_.mod2Read(s->getFd());
		}
		*/
	}

};

}
#endif
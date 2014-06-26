#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include <memory>
namespace simple{
class EventLoop{

private:
	bool acceptCmd_; //whether accept internal command from channel
	//bool acceptSock_; //whether accept coming connetcion
	
	std::function<void(std::string)> cmdhandler_; // 
	
	
	std::shared_ptr<Channel> channelptr_;
	std::shared_ptr<Connector> connectorptr_;
	std::shared_ptr<Acceptor> acceptorptr_;
	std::shared_ptr<IOHandler> iohandlerptr_;
	bool hasPipeEvent();
	bool hasSocketEvent();
	
	std::function<(Connection&)> acceptcb_;
	std::function<void(Connection&)> readcb_;
	std::function<void(Connection&)> writecb_;
	std::function<void(Connection&)> connectcb_;
	EventLoop& operator=(const EventLoop& el){}
	EventLool(const EventLoop& el){}
public:
	EventLoop(Poller& p, Connector& c, Acceptor& a, IOHandler& h);

	
	void loop(){
		while(true){
				
			
		}
	}
};

}
#endif

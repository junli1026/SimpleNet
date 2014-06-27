#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include <memory>
#include "poller.h"

namespace simple{

class EventLoop{
private:
	bool acceptCmd_; //whether accept internal command from channel
	//bool acceptSock_; //whether accept coming connetcion
	Poller poller_;
	Acceptor acceptor_;
	Connector connector_;
	std::shared_ptr<Channel> channelptr_;

	bool hasPipeEvent();
	bool hasSocketEvent();
	void handleCommand();
	void handleSocket();

	std::function<void(std::shared_ptr<Connection>)> acceptcb_;
	std::function<void(std::shared_ptr<Connection>)> connectcb_;
	std::function<void(Connection&)> readcb_;
	std::function<void(Connection&)> writecb_;
	std::function<void(std::string)> cmdhandler_;

	EventLoop& onCommand(std::function<void(std::string)>);
	EventLoop& onAccept(std::function<void(std::shared_ptr<Connection>)>);
	EventLoop& onRead();

	EventLoop& operator=(const EventLoop& el){}
	EventLoop(const EventLoop& el){}

public:
	EventLoop(Poller& p, Connector& c, Acceptor& a, IOHandler& h);
	void loop();
};

}
#endif

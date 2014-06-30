#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <memory>
#include <set>
#include "poller.h"
#include "channel.h"
#include "connection.h"

namespace simple{

enum EventType{
	EPublish,
	EAccept,
	EConnect,
	ERead,
	EWrite
};

class Monitor{
private:
	std::set<int> acceptFds_;
	std::shared_ptr<ConnectionMap> connMap_;
	std::shared_ptr<Poller> poller_;
	std::shared_ptr<Channel> channel_;
	Monitor& operator=(const Monitor& m){}
	Monitor(const Monitor& m){}

public:
	Monitor(std::shared_ptr<Poller> p, 
			std::shared_ptr<Channel> c, 
			std::shared_ptr<ConnectionMap> conns);
	~Monitor();
	void monitorAccept(int fd);
	void monitorRead(std::shared_ptr<Connection> cptr);
	void monitorWrite(std::shared_ptr<Connection> cptr);
	void stopMonitor(std::shared_ptr<Connection> cptr);
	void hasEvent();
	EventType nextEvent();
};

}

#endif
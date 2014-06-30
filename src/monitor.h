#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <memory>
#include <set>
#include "poller.h"
#include "event.h"
#include "connection.h"

namespace simple{

class Monitor{
private:
	std::set<int> acceptFds_;
	Poller poller_;
	bool isAcceptFd(int fd);
	Monitor& operator=(const Monitor& m){}
	Monitor(const Monitor& m){}
	
public:
	Monitor();
	~Monitor();
	void monitorAccept(int fd);
	void monitorRead(std::shared_ptr<Connection> cptr);
	void monitorWrite(std::shared_ptr<Connection> cptr);
	void stopMonitor(std::shared_ptr<Connection> cptr);

	bool hasEvent();
	Event<int> nextEvent(); //Event's data is file descriptor
};

}

#endif

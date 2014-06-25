#ifndef _POLL_H_
#define _POLL_H_

#include <sys/epoll.h>

namespace simple{

const int DefaultEventsSize = 256;

class Poller{
private:
	int epfd_;
	struct epoll_event events_[DefaultEventsSize];
	int ev_count_;
	int ev_index_;

	//void setEpollEvent(struct epoll_event& ev, int fd, bool enableRead, bool enableWrite);
public:
	Poller();
	~Poller();
	void addRead(int fd);
	void addWrite(int fd);
	void addReadWrite(int fd);

	void mod2Write(int fd);
	void mod2Read(int fd);
	void mod2ReadWrite(int fd);

	void remove(int fd);

	void poll(int timeout);
	struct epoll_event* nextEvent();
	bool hasEvent();
};

}

#endif
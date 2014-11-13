#ifndef _WATCHABLE_H_
#define _WATCHABLE_H_

#include <memory>
#include "monitor.h"

namespace simple{

class Watchable{
private:
	int fd_;
	bool readEnabled;
	bool writeEnabled;
	std::shared_ptr<Monitor> monitor_ = nullptr;

	Watchable& operator=(const Watchable& rhs){}
	Watchable(const Watchable& w){}

	void init(){
		this->fd_ = -1;
		this->readEnabled = false;
		this->writeEnabled = false;
		this->monitor_ = nullptr;
	}

protected:
	void setFd(int fd);
	void setMonitor(std::shared_ptr<Monitor> m);

public:
	Watchable();
	Watchable(int fd);
	Watchable(int fd, std::shared_ptr<Monitor> m);
	virtual ~Watchable();

	void enableRead();
	void enableWrite();

	void disableRead();
	void disableWrite();

	int getFd();
};

}
 
#endif
#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include "../connector.h"
#include <vector>

namespace simple{

class Subscriber{
private:
	int fd_;
	Connector connector_;

	Subscriber(const Subscriber&){}
	Subscriber& operator=(const Subscriber&) {}
public:
	void subscribe(const char* host, int port);
	std::vector<uint8_t> receiveData();
	Subscriber();
	~Subscriber();
};

}

#endif

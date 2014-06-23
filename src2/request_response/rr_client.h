#ifndef _RR_CLIENT_
#define _RR_CLIENT_

#include "../connector.h"
#include <vector>

namespace simple{

class RRClient{
private:
	int fd_;
	Connector connector_;
	RRClient(const RRClient&){}
	RRClient& operator=(const RRClient&) {}

public:
	void doConnect(const char* host, int port);
	void sendData(const void* src, size_t sz, bool end);
	std::vector<uint8_t> receiveData();
	RRClient();
	~RRClient();
};

}
#endif
#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <memory>
#include "connection.h"

namespace simple{
	
class Connector{
private:
	Connector& operator=(const Connector& Connector){}
	Connector(const Connector& Connector){}
public:
	std::shared_ptr<Connection> doConnect(const char* host, int port);
	Connector();
	~Connector();
};
}

#endif
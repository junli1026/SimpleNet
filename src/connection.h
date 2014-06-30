#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <memory>
#include <map>
#include "buffer.h"

namespace simple{

enum ConnectionStatus{
	ConnAlive,
	ConnRead,
	ConnWrite,
	ConnClosing,
	ConnClosed,
	ConnError
};

class Connection{
private:
	ConnectionStatus status_;
	bool isClient_;  //connection is connected by client or accepted by server 
	int fd_;
	std::shared_ptr<Buffer> wbufptr_;
	std::shared_ptr<Buffer> rbufptr_;
	
	Connection& operator=(const Connection& c){}
	Connection(const Connection& c){}
	
public:
	Connection(int fd,bool isClient, bool nonblock);
	~Connection();

	std::shared_ptr<Buffer> readBuffer();
	std::shared_ptr<Buffer> writeBuffer();
	void setStatus(ConnectionStatus);
	ConnectionStatus status();
	void setClient();
	void setServer();
	bool isClient();
	bool isServer();
	void doClose();
	void setNonblock();
	int fd();
};

}
#endif

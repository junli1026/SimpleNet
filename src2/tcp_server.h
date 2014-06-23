#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "poller.h"
#include "socket.h"
#include "acceptor.h"
#include "iohandler.h"
#include "channel.h"
#include <map>

namespace simple{

class TcpServer
{
protected:
	Poller poller_;
	Acceptor acceptor_;
	IOHandler iohandler_;
	const TcpServer& operator=(const TcpServer&){}
	TcpServer(const TcpServer&){}

public:
	TcpServer();
	virtual ~TcpServer();
	void monitor(const char* ip, int port);
	virtual void acceptContinuous(int newfd) = 0;
	virtual void readContinuous(int fd) = 0;
	virtual void writeContinuous(int fd) = 0;
	void run();
};

}

#endif

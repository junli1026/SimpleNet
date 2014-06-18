#include "poller.h"
#include "socket.h"
#include "acceptor.h"
#include "io_handler.h"
#include "channel.h"
#include <map>

namespace simple{

class TcpServer
{
protected:
	Poller poller_;
	Acceptor acceptor_;
	std::map<int, std::shared_ptr<ASocket>> acceptSockets_;
	IOHandler iohandler_;
	std::map<int, std::shared_ptr<IOSocket>> ioSockets_;
	
	int doListen(const char*, int, int);
	void doAccept(ASocket*);
	void doWrite(IOSocket*);
	void doRead(IOSocket*);
	bool isAcceptSocket(int fd);


	const TcpServer& operator=(const TcpServer&);
	TcpServer(const TcpServer&);
public:
	TcpServer();
	~TcpServer();
	void monitor(const char* ip, int port);

	virtual void acceptContinuous(const Context& ctx) = 0;
	virtual void readContinuous(const Context& ctx, IOSocket* s) = 0;
	virtual void writeContinuous(const Context& ctx, IOSocket* s) = 0;
	void run();
};

}
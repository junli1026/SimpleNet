#ifndef _RR_SERVER_H_
#define _RR_SERVER_H_

#include "../tcp_server.h"
#include "../message.h"
#include <functional>

namespace simple{

class RRServer: public TcpServer{
private:
	std::function<Message(Message)> response_;
	RRServer(const RRServer& s){}
	RRServer& operator=(const RRServer& s){}

public:
	void acceptContinuous(int newfd) override;
	void readContinuous(int fd) override;
	void writeContinuous(int fd) override;

	void onRequest(std::function<Message(Message)> cb);
	void loop();

	RRServer();
	~RRServer();
};

}
#endif
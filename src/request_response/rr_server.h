#ifndef _RR_SERVER_H_
#define _RR_SERVER_H_

#include "../tcp_server.h"
#include "../block.h"
#include <functional>
#include <memory>

namespace simple{

class RRServer: public TcpServer{
private:
	std::function<Block(std::shared_ptr<Block>)> response_;
	RRServer(const RRServer& s){}
	RRServer& operator=(const RRServer& s){}

public:
	void acceptContinuous(int newfd) override;
	void readContinuous(int fd) override;
	void writeContinuous(int fd) override;

	void onRequest(std::function<Block(std::shared_ptr<Block>)> cb);
	void loop();

	RRServer();
	~RRServer();
};

}
#endif
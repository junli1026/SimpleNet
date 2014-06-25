#ifndef _PUBLISHER_H_
#define _PUBLISHER_H_

#include <vector>
#include <queue>
#
#include "../tcp_server.h"
#include "../channel.h"
#include "../block.h"

namespace simple{

class Publisher: public TcpServer {
private:
	std::string ip_;
	int port_;
	pthread_t serviceId_;
	typedef std::queue<std::shared_ptr<Block>> MessageQueue;
	MessageQueue mq_; //not necessary to be a thread-safe queue
	Channel channel_;
	
	void sendCommand(const std::string& str);
	Publisher(){}
public:
	Publisher(const char* ip, int port);
	~Publisher();
	void loop();

	void runService();
	void stopService();
	void publish(const void* src, size_t sz);
	void acceptContinuous(int newfd) override;
	void writeContinuous(int fd) override;
	void readContinuous(int fd) override;
};

}
#endif


#include "connector.h"
#include <map>
#include <vector>
#include <stdint.h>

namespace simple{

class TcpClient{
private:
	int fd_;
	Connector connector_;
	//Poller poller_;
	//IOHandler iohandler_;
	//std::map<int, std::shared_ptr<IOSocket>> ioSockets_;

	const TcpClient& operator=(const TcpClient&) {}
	TcpClient(const TcpClient&){}
public:
	TcpClient();
	~TcpClient();

	void doConnect(const char* host, int port);
	void sendData(const void* src, size_t sz, bool end);
	std::vector<uint8_t> receiveData();
	//virtual void connectContinuous(int fd);
	//virtual void readContinuous(const Context& ctx) = 0;
	//virtual void writeContinuous(const Context& ctx) = 0;
};

}
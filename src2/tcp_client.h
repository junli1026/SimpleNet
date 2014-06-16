#include "poller.h"
#include "socket.h"
#include "connector.h"
#include "io_handler.h"
#include <map>

namespace simple{

class TcpClient{
private:
	Poller poller_;
	Connector connector_;
	IOHandler iohandler_;
	std::map<int, std::shared_ptr<IOSocket>> ioSockets_;

	const TcpClient& operator=(const TcpClient&) {}
	TcpClient(const TcpClient&){}
public:
	TcpClient();
	~TcpClient();

	void doConnect(const char* host, int port);

	virtual void connectContinuous(int fd);
	virtual void readContinuous(const Context& ctx) = 0;
	virtual void writeContinuous(const Context& ctx) = 0;
};

}
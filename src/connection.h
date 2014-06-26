#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <memory>
#include "buffer.h"

namespace simple{

enum ConnectionStatus{
  ConnAlive,
  ConnClosing,
  ConnClosed,
  ConnError
};

class Connection{
private:
	ConnectionStatus status_;
	bool readable_;
	bool writeable_;
	bool isClient_;  //connection is connected by client or accepted by server 
	std::shared_ptr<Buffer> wbufptr_;
	std::shared_ptr<Buffer> rbufptr_;
	int fd_;

	Connection& operator=(const Connection& c){}
	Connection(const Connection& c){}
	
public:
	Connection(int fd, bool enableRead, bool enableWrite, bool isClient);
	~Connection();

  std::shared_ptr<Buffer> readBuffer();
  std::shared_ptr<Buffer> writeBuffer();
  ConnectionStatus status();
  bool isClient();
  bool isServer();
  void close();
};

}
#endif

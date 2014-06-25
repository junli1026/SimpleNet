#ifndef _IOHANDLER_H_
#define _IOHANDLER_H_

#include <functional>
#include <memory>
#include <map>
#include <unistd.h>
#include "socket.h"
#include "buffer.h"

namespace simple{

class IOHandler{
private:
	std::map<int, std::shared_ptr<IOSocket>> sockets_;
	
	IOHandler& operator=(const IOHandler& IOHandler){}
	IOHandler(const IOHandler& IOHandler){}
public:
	bool contains(int fd);
	void add(int fd);
	void erase(int fd);
	void doRead(int fd);
	void doWrite(int fd);	
	std::shared_ptr<Buffer> getReadBuffer(int fd);
	std::shared_ptr<Buffer> getWriteBuffer(int fd);
	std::map<int, std::shared_ptr<IOSocket>>& getSocketMap();
	
	IOHandler();
	~IOHandler();
};

}
#endif

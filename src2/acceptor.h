#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <memory>
#include <map>
#include "socket.h"

namespace simple{

class Acceptor{
private:
	std::map<int, std::shared_ptr<ASocket>> sockets_;
	
	Acceptor& operator=(const Acceptor& acceptor){}
	Acceptor(const Acceptor& acceptor){}

public:
	bool contains(int fd);
	void erase(int fd);
	void add(int fd);
	int doAccept(int fd); //return new built connection fd
	
	Acceptor();
	~Acceptor();
};

}

#endif

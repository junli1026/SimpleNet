#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <memory>
#include <set>
#include "connection.h"

namespace simple{

class Acceptor{
private:
	std::set<int> fds_;
	
	Acceptor& operator=(const Acceptor& acceptor){}
	Acceptor(const Acceptor& acceptor){}

public:
	bool contains(int fd);
	void erase(int fd);
	void add(int fd);
	std::shared_ptr<Connection> doAccept(int fd); //return new built connection fd
	
	Acceptor();
	~Acceptor();
};

}

#endif

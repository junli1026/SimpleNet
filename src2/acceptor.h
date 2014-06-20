#include "context.h"
#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h> 

namespace simple{

class Acceptor{
private:
	Context ctx_;

	const Acceptor& operator=(const Acceptor& acceptor){}
	Acceptor(const Acceptor& acceptor){}
public:
	const Context& doAccept(ASocket* s){
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		int connfd = accept(s->getFd(), (struct sockaddr*)&client_addr, &len);
		if(connfd < 0){
			s->setClose();
			this->ctx_.connfd = connfd;
			this->ctx_.operation = SockOpAccept;
			this->ctx_.errnumber = errno;
			return this->ctx_;
		}
		this->ctx_.errnumber = 0;
		this->ctx_.connfd = connfd;
		this->ctx_.operation = SockOpAccept;
		return this->ctx_;
	}
	Acceptor(){}
	~Acceptor(){}
};

class Acceptor{
private:
	std::shared_ptr<Poller> poller_;
	std::map<int, std::shared_ptr<ASocket>> sockets_;
	Acceptor& operator=(const Acceptor& acceptor){}
	Acceptor(const Acceptor& acceptor){}
public:
	Acceptor(std::shared_ptr<Poller> poller);
	~Acceptor();
	bool contains(int fd);
	void erase(int fd);
	int doAccept(int fd); //return new built connection fd
}



}

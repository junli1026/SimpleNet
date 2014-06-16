#ifndef _SOCK_CONTEXT_H_
#define _SOCK_CONTEXT_H_

namespace simple{
enum SockOp{
	SockOpAccept,
	SockOpRead, 
	SockOpWrite
};

struct Context{
	int connfd;
	SockOp operation;
	int errnumber;
};
}
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

class Connector{
private:
	const Connector& operator=(const Connector& Connector){}
	Connector(const Connector& Connector){}
public:
	int doConnect(const char* host, int port){
		uint32_t internet_addr;
	    if (host[0]) {
	        internet_addr = inet_addr(host);
	    }else{
	        internet_addr = INADDR_ANY;
	    }
	    //set socket address
	    struct sockaddr_in socket_addr;
	    memset(&socket_addr, 0, sizeof(struct sockaddr_in));
	    socket_addr.sin_family = AF_INET;
	    socket_addr.sin_port = htons(port);
	    socket_addr.sin_addr.s_addr = internet_addr;

	    int connect_fd = socket(AF_INET, SOCK_STREAM, 0); //tcp
	    if (connect_fd < 0) {
	        return -1;
	    }

	    if(connect(connect_fd, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr)) < 0){
	        return -1;
	    }  
	    return connect_fd;
	}

	Connector(){}
	~Connector(){}
};

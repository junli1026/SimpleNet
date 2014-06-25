int doListen(const char *host, int port, int backlog){
	uint32_t internet_addr; 
	if (host[0]) {
	    internet_addr = inet_addr(host);
	}else{
	    internet_addr = INADDR_ANY; //default: listen to all IPs on this machine
	}
	//set socket address
	struct sockaddr_in socket_addr;
	memset(&socket_addr, 0, sizeof(struct sockaddr_in));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	socket_addr.sin_addr.s_addr = internet_addr;

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0); //tcp
	if (listen_fd < 0) {
	    return -1;
	}

	//set SO_REUSEADDR, so that our server process can quickly reboot, even if socket is still in TIME_WAIT status
	int reuse = 1;
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(int)) == -1) {
	    return -1;
	}

	if(bind(listen_fd, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr)) == -1) {
	    return -1;
	}
	if(listen(listen_fd, backlog) == -1) {
	    return -1;
	}
	return listen_fd;
}
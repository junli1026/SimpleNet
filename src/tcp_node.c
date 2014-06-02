#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>
#include <pthread.h>

#include "buffer.c"
#include "address_book.c"
#include "channel.c"

#define MAX_EPOLL_EVENTS 64
#define MAX_ADDRESS 32

struct command{
	char type;
	char addr[MAX_ADDRESS];	
	struct {
		void* data;
		size_t sz;
	} d;
};

void command_init(struct command *c, char type, const char* addr, size_t sz){
	memset(c, 0, sizeof(*c));
	c->type = type;
	sz = (sz > MAX_ADDRESS-1)? MAX_ADDRESS-1 : sz;
	memcpy(c->addr, addr, sz);
	c->d.data = NULL;
	c->d.sz = 0;
}

#define SOCKET_VALID 1
#define SOCKET_LISTENED 2
#define SOCKET_WRITE 4  //successfully connected to other node, work as a client
#define SOCKET_READ 4  //successfully connected to other node, work as a client

struct socket{
	int status;
	int fd;
	struct buffer* buf;
};

#define BACKLOG 1024
#define MAX_SOKCET 256

struct tcp_node{
	struct channel* cc;
	struct buffer* cmdbuf;

	struct address_book *ab;
	int epollfd;
	struct socket slots[MAX_SOKCET];
};

struct tcp_node* tcp_node_new(){
	struct tcp_node* n = malloc(sizeof(*n));
	if(n == NULL){
		return NULL;
	}
	n->cc = channel_new();
	n->cmdbuf = buffer_new(512);
	n->ab = address_book_new();
	n->epollfd = epoll_create(1024);
 	int i = 0;
	for(i = 0; i < MAX_SOKCET; i++){
		n->slots[i].status = SOCKET_VALID;
		n->slots[i].fd = -1;
		n->slots[i].buf = buffer_new(128);
	}
	return n;
}

void tcp_node_delete(struct tcp_node** node_addr){
	if(node_addr){
		struct tcp_node* n = *node_addr;
		if(n){
			buffer_delete(&n->cmdbuf);
			int i;
			for(i = 0; i < MAX_SOKCET; i++){
				if(n->slots[i].fd > 0){
					close(n->slots[i].fd);
				}
				if(n->slots[i].buf){
					buffer_delete(&n->slots[i].buf);
				}
			}
			address_book_delete(&n->ab);
			channel_delete(&n->cc);
		}
		free(n);
		*node_addr = NULL;
	}
}

static int send_data(struct tcp_node* node, void* data, size_t len){
	int n = write(node->cc->writefd, &len, sizeof(len));
	if(n != sizeof(len)){
		return -1;
	}
	n = write(node->cc->writefd, data, len);
	if(n != len){
		return -1;
	}
	return 0;
}

static int send_cmd(struct tcp_node* node, char type, const char* addr, size_t sz, void* data, size_t datalen){
	struct command c;
	command_init(&c, type, addr, sz);
	if(type == 'D'){
		c.d.data = malloc(datalen);
		memcpy(c.d.data, data, datalen);
		c.d.sz = datalen;
	}

	int n = write(node->cc->writefd, &c, sizeof(c));
	if(n < 0){
		perror("write");
		return -1;
	}
	if(n != sizeof(c)){
		return -1;
	}
	return 0;
}

int tcp_node_data(struct tcp_node* node, const char* addr, void* data, size_t sz){
	int fd = ab_find_fd(node->ab, addr, strlen(addr));
	if(fd < 0){
		printf("address %s not reachable.\n", addr);
		return -1;
	}
	return send_cmd(node, 'D', addr, strlen(addr), data, sz);
}

int tcp_node_listen(struct tcp_node* node, const char* addr){
	return send_cmd(node, 'L', addr, strlen(addr), NULL, 0);
}

int tcp_node_connect(struct tcp_node* node, const char* addr){
	return send_cmd(node, 'C', addr, strlen(addr), NULL, 0);
}

char* get_ip(char addr[], int* port){
	char* a = strtok(addr, ":");
	if(a != NULL){
		char* b = strtok(NULL, ":");
		*port = atoi(b);
	}
	return a;
}

// return listen fd
static int do_listen(const char *host, int port){
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
    if(listen(listen_fd, BACKLOG) == -1) {
        return -1;
    }
    return listen_fd;
}

static int do_connect(const char *host, int port){
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
        perror("connect");
        return -1;
    }  
    return connect_fd;
}


static int read_data(int fd, struct buffer* buf){
	int sz = 512;
	int tmp[sz];
	int n = read(fd, tmp, sz);
	if(n < 0){
		perror("read");
		return -1;
	}
	return buffer_append(buf, tmp, n);
}

static char command_type(struct tcp_node* node){
	size_t sz;
	void* data = buffer_data(node->cmdbuf, &sz);
	if(sz >= sizeof(struct command)){
		struct command* cmd = (struct command*)data;
		return cmd->type;
	}
	return '\0';
}

static int handle_listen_cmd(struct tcp_node* node){
	size_t sz;
	struct command* cmd = buffer_data(node->cmdbuf, &sz);
	int port;
	char* ip = get_ip(cmd->addr, &port);

	int listenfd = do_listen((const char*)ip, port);
	if(listenfd <= 0){
		return -1;
	}
	struct socket* s = &node->slots[listenfd % MAX_SOKCET];
	if(s->status == SOCKET_VALID){
		s->fd = listenfd;
		s->status = SOCKET_LISTENED;
		set_fd_nonblocking(s->fd);
		if(watch_read(node->epollfd, s->fd)){
			goto _fail;
		}
		buffer_truncate(node->cmdbuf, sizeof(struct command));
		return 0;
	}

_fail:
	close(listenfd);
	buffer_truncate(node->cmdbuf, sizeof(struct command));
	return -1;
}

static int handle_connect_cmd(struct tcp_node* node){
	size_t sz;
	struct command* cmd = buffer_data(node->cmdbuf, &sz);
	int port;
	char* ip = get_ip(cmd->addr, &port);
	int connectfd = do_connect((const char*)ip, port);
	if(connectfd <= 0){
		return -1;
	}
	char tmp_addr[MAX_ADDRESS] = {'\0'};
	sprintf(tmp_addr, "%s:%i", ip, port);
	printf("ab_add_address: %s\n", tmp_addr);
	ab_add_address(node->ab, tmp_addr, MAX_ADDRESS-1, connectfd);
	struct socket* s = &node->slots[connectfd % MAX_SOKCET];
	if(s->status == SOCKET_VALID){
		s->fd = connectfd;
		s->status = SOCKET_WRITE;
		set_fd_nonblocking(s->fd);
		if(watch_write(node->epollfd, s->fd)){
			goto _fail;
		}
		buffer_truncate(node->cmdbuf, sizeof(struct command));
		return 0;
	}

_fail:
	close(connectfd);
	buffer_truncate(node->cmdbuf, sizeof(struct command));
	return -1;
}

static int handle_data_cmd(struct tcp_node* node){
	size_t sz;
	struct command* cmd = buffer_data(node->cmdbuf, &sz);
	int fd = ab_find_fd(node->ab, cmd->addr, MAX_ADDRESS -1);
	if(fd < 0){
		return -1;
	}
	printf("ab_find_fd: %s, get %i\n", cmd->addr, fd);

	void* data = cmd->d.data;
	size_t datalen = cmd->d.sz;
	buffer_truncate(node->cmdbuf, sizeof(struct command));
	struct socket* s = &node->slots[fd % MAX_SOKCET];

	buffer_append(s->buf, &datalen, sizeof(size_t));
	buffer_append(s->buf, data, datalen);
	printf("append %i bytes\n", datalen);	
	int i;
	for(i = 0; i < datalen; i++){
		printf("%c", *((char*)data + i));
	}
	printf("\n");
	free(data);
	/*
	if(s->status == SOCKET_WRITE){
		buffer_append(s->buf, &datalen, sizeof(size_t));
		buffer_append(s->buf, data, datalen);
		printf("append %i bytes\n", datalen);	
		int i;
		for(i = 0; i < datalen; i++){
			printf("%c", *((char*)data + i));
		}
		printf("\n");
		free(data);
	}else{
		printf("status not valid\n");
		free(data);
	}
	*/
	return 0;
	//debug
	/*
	int i = 0;
	printf("received %i bytes data: \n", datalen);
	for(i = 0; i < datalen; i++){
		printf("%c", *((char*)data + i));
	}
	printf("\n");
	*/
}

static int handle_cmd(struct tcp_node* node){
	struct channel* cc = node->cc;
	if(channel_has_data(cc)){
		if(read_data(cc->readfd, node->cmdbuf)){
			return -1;
		}

		char type = command_type(node);
		int ret;
		switch(type){
			case 'L':
				ret = handle_listen_cmd(node);
				break;
			case 'C':
				ret = handle_connect_cmd(node);
				break;
			case 'D':
				ret = handle_data_cmd(node);
				break;
			default:
				break;
		}
		return ret;
	}
	return 0;
}

static void do_accpet(struct tcp_node* node, int listenfd){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &len);

	if(connfd >= MAX_SOKCET){
		printf("connfd too big\n");
		close(connfd);
		return;
	}
	if(watch_read(node->epollfd, connfd)){
		close(connfd);
		return;
	}
	struct socket* s = &node->slots[connfd % MAX_SOKCET];
	s->status = SOCKET_READ;
	s->fd = connfd;

	/*
	char addr[MAX_ADDRESS] = {'\0'};
	const char* ip = inet_ntop(AF_INET, &(client_addr.sin_addr), addr, MAX_ADDRESS-1);
	sprintf(addr + strlen(ip), ":%i", client_addr.sin_port);
	printf("ab_add_address: %s\n", addr);
	ab_add_address(node->ab, (const char*)addr, MAX_ADDRESS, s->fd);
	*/
}

void tcp_node_loop(struct tcp_node* node){
	while(1){
		int error = handle_cmd(node);
		if(error){
			//error
		}
		struct epoll_event events[MAX_EPOLL_EVENTS];
		int n = epoll_wait(node->epollfd, events, MAX_EPOLL_EVENTS, 100); //timeout = 0.1 sec
		struct socket* s = NULL;
		int i = 0, fd = -1;
		for(i = 0; i < n; i++){
			fd = events[i].data.fd;
			s = &node->slots[fd % MAX_SOKCET];
			if(s->status == SOCKET_LISTENED){
				do_accpet(node, fd);
			}else if(s->status == SOCKET_READ && (events[i].events & EPOLLIN)){ //received data from clients
				char buf[512];
				int n = read(s->fd, buf, 512);
				if(n > 0){
					buffer_append(s->buf, buf, n);
					for(i = 0; i < n; i++){
						printf("%c", buf[i]);
					}
					printf("\n");
				}else{
					printf("read return %i\n", n);
				}
			}else if(s->status == SOCKET_WRITE && (events[i].events & EPOLLOUT)){ //ready to send data
				size_t sz;
				void* data = buffer_data(s->buf, &sz);
				if(data){
					sz = sz > 512 ? 512 : sz;
					printf("sz : %u\n", sz);
					int n = write(s->fd, data, sz);
					if(n > 0){
						printf("write %i bytes.\n", n);
						buffer_truncate(s->buf, n);
					}else{
						printf("write return %i\n", n);
					}
				}else{
					//printf("write buffer empty\n");
				}
			}else{
				printf("close fd %i\n", events[i].data.fd);
				epoll_ctl(node->epollfd, EPOLL_CTL_DEL, fd, NULL);
				close(events[i].data.fd);
			}
		}
	}
}

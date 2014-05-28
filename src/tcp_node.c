#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>
#include "address_book.c"
#include "channel.c"

struct command{
	char type;
	char addr[MAX_ADDRESS];
};

void command_init(struct command *c, char type, const char* addr, size_t sz){
	memset(c, 0, sizeof(*c));
	c->type = type;
	sz = (sz > MAX_ADDRESS-1)? MAX_ADDRESS-1 : sz;
	memcpy(c->addr, addr, sz);
}

#define SOCKET_VALID 1
#define SOCKET_LISTENED 2
#define SOKCET_CONNECTED 4  //successfully connected to other node, work as a client

struct socket{
	int status;
	int fd;
};

#define BACKLOG 1024
#define MAX_SOKCET 1024

struct tcp_node{
	struct channel* cc;
	struct address_book *ab;
	int epollfd;
	int listenfd;
	struct socket slots[MAX_SOKCET];
};

struct tcp_node* tcp_node_new(){
	struct tcp_node* n = malloc(sizeof(*n));
	if(n == NULL){
		return NULL;
	}
	n->cc = channel_new();
	n->ab = address_book_new();
	n->epollfd = epoll_create(1024);
	n->listenfd = -1;
 	int i = 0;
	for(i = 0; i < MAX_SOKCET; i++){
		n->slots[i].status = SOCKET_VALID;
		n->slots[i].fd = -1;
	}
}

void tcp_node_delete(struct tcp_node** node_addr){
	if(node_addr){
		struct tcp_node* n = *node_addr;
		if(n){
			close(n->listenfd);
			int i;
			for(i = 0; i < MAX_SOKCET; i++){
				if(n->slots[i].fd > 0){
					close(n->slots[i].fd);
				}
			}
			address_book_delete(&n->ab);
			channel_delete(&n->cc);
			free(n);
		}
		*node_addr = NULL;
	}
}

static int send_cmd(struct tcp_node* node, char type, const char* addr, size_t sz){
	struct command c;
	command_init(&c, type, addr, sz);
	int n = write(node->cc->writefd, &c, sizeof(c));
	if(n < 0){
		perror("write");
		return -2;
	}
	if(n != sizeof(c)){
		channel_delete(&node->cc);
		node->cc = channel_new();
		return -1;
	}
	return 0;
}

int send_listen(struct tcp_node* node, const char* addr){
	return send_cmd(node, 'L', addr, strlen(addr));
}

int send_connect(struct tcp_node* node, const char* addr){
	return send_cmd(node, 'C', addr, strlen(addr));
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

int handle_cmd(struct tcp_node* node){
	struct channel* cc = node->cc;
	if(channel_has_data(cc)){
		struct command cmd;
		int n = read(cc->readfd, &cmd, sizeof(cmd));
		if(n < 0){
			perror("read");
			return -2;
		}
		if(n != sizeof(cmd)){
			return -1;
		}

		int port;
		char* ip = get_ip(cmd.addr, &port);
		struct socket* s = NULL;
		int listenfd;
		int connectfd;
		switch(cmd.type){
			case 'L':
				listenfd = do_listen((const char*)ip, port);
				if(listenfd <= 0){
					break;
				}
				s = &node->slots[listenfd % MAX_SOKCET];
				if(s->status == SOCKET_VALID){
					s->fd = listenfd;
					s->status = SOCKET_LISTENED;
					set_fd_nonblocking(s->fd);
					if(watch_read(node->epollfd, s->fd)){
						perror("watch_read");
						close(listenfd);
					}
				}else{
					close(listenfd);
				}
				break;
			case 'C':
				connectfd = do_connect((const char*)ip, port);
				if(connectfd <= 0){
					break;
				}
				s = &node->slots[connectfd % MAX_SOKCET];
				if(s->status == SOCKET_VALID){
					s->fd = connectfd;
					s->status = SOKCET_CONNECTED;
					set_fd_nonblocking(s->fd);
					if(watch_write(node->epollfd, s->fd)){
						perror("watch_write");
						close(listenfd);
					}
				}else{
					close(listenfd);
				}
				break;
			default:
				break;
		}
		return 0;
	}
	return 0;
}

int main(){
	struct tcp_node * n = tcp_node_new();
	send_listen(n, "127.0.0.1:1234");
	send_connect(n, "127.0.0.1:1235");
	handle_cmd(n);
	handle_cmd(n);
	tcp_node_delete(&n);
}
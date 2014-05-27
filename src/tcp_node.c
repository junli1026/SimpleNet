#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <assert.h>
#include "channel.c"

struct command{
	char type;
	int port;
	char ip[32];
};

void command_init(struct command *c){
	c->type = '\0';
	c->port = -1;
	memset(c->ip, '\0', 32);
}

#define SOCKET_OPEN 1
#define SOKCET_CLOSED 2
#define SOCKET_LISTENED 4
#define SOKCET_CONNECTED 8  //successfully connected to other node, work as a client

struct socket{
	int type;
	int fd;
};

#define BACKLOG 1024
#define MAX_SOKCET 1024

struct tcp_node{
	struct channel* cc;
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
	n->epollfd = -1;
	n->listenfd = -1;
 	int i = 0;
	for(i = 0; i < MAX_SOKCET; i++){
		n->slots[i].type = SOCKET_OPEN;
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
				close(n->slots[i].fd);
			}
			channel_delete(&n->cc);
			free(n);
		}
		*node_addr = NULL;
	}
}

int send_cmd(struct tcp_node* node, char type, int port, char* ip, size_t sz){
	struct command c;
	command_init(&c);
	memcpy(c.ip, ip, sz);
	int n = write(node->cc, &c, sizeof(c));
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
		switch(cmd.type){
			case 'L':
				node->listenfd = do_listen((const char*)cmd.ip, cmd.port, 1024);
				if(node->listenfd > 0){
					node->listenfd = ;
				}
				break;
			case 'C':
				do_connect();
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
	tcp_node_delete(&n);
}
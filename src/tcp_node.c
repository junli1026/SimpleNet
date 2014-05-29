#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>
#include <pthread.h>
#include "address_book.c"
#include "channel.c"

#define MAX_EPOLL_EVENTS 64

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

struct buffer{
	size_t length;
	void* data;
	struct buffer* next;
};

struct buffer_list{
	struct buffer* head;
	struct buffer* tail;
};

struct buffer_list* buffer_list_new(){
	struct buffer_list* bl = malloc(sizeof(*bl));
	if(bl){
		bl->head = NULL;
		bl->tail = NULL;
	}
	return bl;
}

void buffer_list_delete(struct buffer_list** bl_addr){
	if(bl_addr){
		struct buffer_list* bl = *bl_addr;
		if(bl){
			struct buffer* b = bl->head;
			struct buffer* tmp = NULL;
			while(b){
				tmp = b->next;
				if(b->data){
					free(b->data);
				}
				free(b);
				b = tmp;
			}
		}
		free(bl);
		*bl_addr = NULL;
	}
}

void bl_append(struct buffer_list* bl, void* data, size_t l){
	assert(bl);
	struct buffer* b = malloc(sizeof(*b));
	if(b){
		b->length = l;
		b->next = NULL;
		b->data = data;
		if(bl->head == NULL){
			bl->head = b;
			bl->tail = b;
		}else{
			bl->tail->next = b;
			bl->tail = b;
		}
	}
}

void* bl_remove_head(struct buffer_list* bl, size_t* l){
	assert(bl);
	if(bl->head == NULL){
		*l = 0;
		return NULL;
	}else{
		struct buffer* h = bl->head;
		bl->head = bl->head->next;
		*l = h->length;
		void* ret = h->data;
		free(h);
		return ret;
	}
}

#define SOCKET_VALID 1
#define SOCKET_LISTENED 2
#define SOCKET_WRITE 4  //successfully connected to other node, work as a client
#define SOCKET_READ 4  //successfully connected to other node, work as a client

struct socket{
	int status;
	int fd;
	struct buffer_list* bl;
};

#define BACKLOG 1024
#define MAX_SOKCET 256

struct tcp_node{
	struct channel* cc;
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
	n->ab = address_book_new();
	n->epollfd = epoll_create(1024);
 	int i = 0;
	for(i = 0; i < MAX_SOKCET; i++){
		n->slots[i].status = SOCKET_VALID;
		n->slots[i].fd = -1;
		n->slots[i].bl = buffer_list_new();
	}
	return n;
}

void tcp_node_delete(struct tcp_node** node_addr){
	if(node_addr){
		struct tcp_node* n = *node_addr;
		if(n){
			int i;
			for(i = 0; i < MAX_SOKCET; i++){
				if(n->slots[i].fd > 0){
					close(n->slots[i].fd);
				}
				buffer_list_delete(&n->slots[i].bl);
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
	int n = write(node->cc->writefd, &c, sizeof(c));
	if(n < 0){
		perror("write");
		return -1;
	}
	if(n != sizeof(c)){
		return -1;
	}

	if(type == 'D'){
		if(send_data(node, data, datalen)){
			return -1;
		}
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

int handle_cmd(struct tcp_node* node){
	struct channel* cc = node->cc;
	if(channel_has_data(cc)){
		struct command cmd;
		int n = read(cc->readfd, &cmd, sizeof(cmd));
		if(n < 0){
			perror("read");
			return -1;
		}

		int port;
		char* ip = NULL;
		struct socket* s = NULL;
		if(cmd.type == 'L'){
			ip = get_ip(cmd.addr, &port);
			int listenfd = do_listen((const char*)ip, port);
			if(listenfd <= 0){
				return -1;
			}
			s = &node->slots[listenfd % MAX_SOKCET];
			if(s->status == SOCKET_VALID){
				s->fd = listenfd;
				s->status = SOCKET_LISTENED;
				set_fd_nonblocking(s->fd);
				if(watch_read(node->epollfd, s->fd)){
					close(listenfd);
				}
			}else{
				close(listenfd);
			}
		}else if(cmd.type == 'C'){
			ip = get_ip(cmd.addr, &port);
			int connectfd = do_connect((const char*)ip, port);
			if(connectfd <= 0){
				return -1;
			}
			char tmp_addr[MAX_ADDRESS] = {'\0'};
			sprintf(tmp_addr, "%s:%i", ip, port);
			printf("ab_add_address: %s\n", tmp_addr);
			ab_add_address(node->ab, tmp_addr, MAX_ADDRESS - 1, connectfd);
			s = &node->slots[connectfd % MAX_SOKCET];
			if(s->status == SOCKET_VALID){
				s->fd = connectfd;
				s->status = SOCKET_WRITE;
				set_fd_nonblocking(s->fd);
				if(watch_write(node->epollfd, s->fd)){
					close(connectfd);
				}
			}else{
				close(connectfd);
			}
		}else if(cmd.type == 'D'){
			size_t len;
			read(cc->readfd, &len, sizeof(len));
			void* d = malloc(len);
			read(cc->readfd, d, len);
			int fd = ab_find_fd(node->ab, cmd.addr, MAX_ADDRESS -1);
			printf("ab_find_fd: %s, get %i\n", cmd.addr, fd);

			struct buffer_list* bl = node->slots[fd % MAX_SOKCET].bl;
			bl_append(bl, d, len);

			int i = 0;
			printf("received %i bytes data: \n", len);
			for(i = 0; i < len; i++){
				printf("%c", *((char*)d + i));
			}
			printf("\n");
		}else{

		}
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
				printf("fd %i has data\n", fd);
			}else if(s->status == SOCKET_WRITE && (events[i].events & EPOLLOUT)){ //ready to send data
				printf("fd %i ready to write\n", fd);
			}else{
				printf("close fd %i\n", events[i].data.fd);
				epoll_ctl(node->epollfd, EPOLL_CTL_DEL, fd, NULL);
				close(events[i].data.fd);
			}
		}
	}
}


void* server(void* data){
	struct tcp_node* n = (struct tcp_node*) data;
	tcp_node_loop(n);
}

int main(){
	pthread_t th;

	struct tcp_node * n = tcp_node_new();
	
	pthread_create(&th, NULL, server, (void*)n);

	sleep(1);
	tcp_node_listen(n, "127.0.0.1:1234");
	tcp_node_loop(n);

	tcp_node_connect(n, "127.0.0.1:1234");
	tcp_node_loop(n);

	tcp_node_data(n, "127.0.0.1:1234", "helloworld", strlen("helloworld"));
	tcp_node_loop(n);
	
	tcp_node_delete(&n);
}

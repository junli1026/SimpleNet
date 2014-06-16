#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <assert.h>

struct channel{
	int readfd;
	int writefd;
	int epollfd;
};

static void set_fd_nonblocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if (-1 == flag) {
        printf("set_fd_nonblocking failed.\n");
        return;
    }
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

static inline int watch_read(int epfd, int fd){
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int error = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("watch_read");
	}
	return error;
}

static inline int watch_write(int epfd, int fd){
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	int error = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(error){
		perror("watch_write");
	}
	return error;
}

struct channel* channel_new(){
	struct channel* c = malloc(sizeof(struct channel));
	if(c == NULL){
		return NULL;
	}

	int fd[2];
	if(pipe(fd)){
		free(c);
		return NULL;
	}
    c->readfd = fd[0]; //read out
    c->writefd = fd[1]; //write in
    set_fd_nonblocking(c->readfd);
    set_fd_nonblocking(c->writefd);

    c->epollfd = epoll_create(1024);
    if(c->epollfd < 0){
    	close(c->readfd);
    	close(c->writefd);
    	free(c);
    	return NULL;
    }

    if(watch_read(c->epollfd, c->readfd)){
    	close(c->readfd);
    	close(c->writefd);
    	free(c);
    	return NULL;
    }
    return c;
}

void channel_delete(struct channel** c_addr){
	if(c_addr){
		struct channel* c = *c_addr;
		if(c){
			free(c);
		}
		*c_addr = NULL;
	}
}

int channel_has_data(struct channel *c){
	struct epoll_event events[1];
	int n = epoll_wait(c->epollfd, events, 1, 0);
	if(n > 0){
		assert(n == 1);
		assert(events[0].data.fd == c->readfd);
		return 1;
	}else{
		return 0;
	}
}

/*
int main(){
	struct channel* c = channel_new();
	if(c){
		write(c->writefd, "hello", strlen("hello"));
		if(channel_has_data(c)){
			char buf[1024] = {'\0'};
			int n = read(c->readfd, buf, 1024);
			printf("read %i bytes\n", n);
			printf("%s\n", buf);
		}
		channel_delete(&c);
	}
}
*/

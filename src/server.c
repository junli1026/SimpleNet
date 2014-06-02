#include "tcp_node.c"

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

	sleep(20);
	tcp_node_delete(&n);
}

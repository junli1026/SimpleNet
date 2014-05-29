#ifndef _TCP_NODE_
#define _TCP_NODE_

struct tcp_node;

struct tcp_node* tcp_node_new();

void tcp_node_delete(struct tcp_node** node_addr);

int tcp_node_data(struct tcp_node* node, const char* addr, void* data, size_t sz);

int tcp_node_listen(struct tcp_node* node, const char* addr);

int tcp_node_connect(struct tcp_node* node, const char* addr);

void tcp_node_loop(struct tcp_node* node);

#endif
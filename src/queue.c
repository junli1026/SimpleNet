#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//FIFO queue, implemented by linked list.

struct queue_node{
	void* data;
	size_t sz;
	struct queue_node* next;
};

struct queue{
	struct queue_node* head;
	struct queue_node* tail;
};

struct queue* queue_new(){
	struct queue* q = malloc(sizeof(*q));
	if(q){
		q->head = NULL;
		q->tail = NULL;
	}
}

void queue_delete(struct queue** q_addr){
	if(q_addr){
		struct queue* q = *q_addr;
		if(q){
			struct queue_node* n = q->head;
			struct queue_node* tmp = NULL;
			while(n){
				tmp = n->next;
				if(n->data){
					free(n->data);
				}
				free(n);
				n = tmp;
			}
			free(q);
		}
		*q_addr = NULL;
	}
}

int queue_empty(struct queue* q){
	if(q && q->head != NULL){
		return 0;
	}else{
		return 1;
	}
}

void enqueue(struct queue* q, void* data, size_t len){
	struct queue_node* n = malloc(sizeof(*n));
	if(n == NULL){
		perror("malloc");
		return;
	}
	n->data = data;
	n->sz = len;
	n->next = NULL;
	if(q->head == NULL){
		q->head = n;
		q->tail = n;
	}else{
		q->tail->next = n;
		q->tail = n;
	}
}

void* dequeue(struct queue* q, size_t* len){
	if(queue_empty(q)){
		*len = 0;
		return NULL;
	}
	void* ret = q->head->data;
	*len = q->head->sz;
	struct queue_node* next = q->head->next;
	free(q->head);
	if(next == NULL){
		q->head = NULL;
		q->tail = NULL;
	}else{
		q->head = next;
	}
	return ret;
}

/*
int main(){
	struct queue* q = queue_new();
	int a = 12;
	int b = 13;
	enqueue(q, &a, sizeof(int));
	enqueue(q, &b, sizeof(int));

	size_t sz;
	int* t = dequeue(q, &sz);
	assert(t == &a);
	assert(sz == sizeof(int));

	t = dequeue(q, &sz);
	assert(t == &b);
	assert(sz == sizeof(int));
	queue_delete(&q);
}
*/
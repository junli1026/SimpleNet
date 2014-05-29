#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRY 512
#define MAX_ADDRESS 32

struct address_book{
	struct address* entries[MAX_ENTRY];
};

struct address{
	char key[MAX_ADDRESS];
	int fd;
	struct address* next;
};

unsigned long hash(const char* str){
	unsigned long hash = 5381;
	int c;
	while(c = *str++){
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

struct address_book* address_book_new(){
	struct address_book* t = malloc(sizeof(*t));
	if(t){
		int i;
		for(i = 0; i < MAX_ENTRY; i++){
			t->entries[i] = NULL;
		}
	}
	return t;
}

void address_book_delete(struct address_book** book_addr){
	if(book_addr){
		struct address_book* b = *book_addr;
		if(b){
			struct address* a = NULL;
			struct address* tmp = NULL;
			int i;
			for(i = 0; i < MAX_ENTRY; i++){
				a = b->entries[i];
				while(a){
					tmp = a->next;
					free(a);
					a = tmp;
				}	
			}
		}
		free(b);
	}
	*book_addr = NULL;
}

void ab_add_address(struct address_book* b, const char* key, size_t sz, int fd){
	if(b == NULL || key == NULL){
		return;
	}
	char k[MAX_ADDRESS] = {'\0'};
	size_t s = (sz > (MAX_ADDRESS -1))? MAX_ADDRESS - 1 : sz;
	memcpy(k, key, s);
	int i = hash(k) % MAX_ENTRY;
	struct address* addr = malloc(sizeof(*addr));
	if(addr == NULL){
		perror("malloc\n");
		return;
	}
	memcpy(addr->key, k, MAX_ADDRESS);
	addr->next = b->entries[i];
	addr->fd = fd;
	b->entries[i] = addr;
}

struct address* ab_find_address(struct address_book* b, const char* key, size_t sz){
	if(b == NULL || key == NULL){
		return NULL;
	}
	char k[MAX_ADDRESS] = {'\0'};
	size_t s = (sz > (MAX_ADDRESS -1))? MAX_ADDRESS - 1 : sz;
	memcpy(k, key, s);
	struct address* addr = b->entries[hash(k) % MAX_ENTRY];
	while(addr){
		if(strcmp(addr->key, k) == 0){
			return addr;
		}
		addr = addr->next;
	}
	return NULL;
}

int ab_find_fd(struct address_book* b, const char* key, size_t sz){
	struct address* a = ab_find_address(b, key, sz);
	if(a != NULL){
		return a->fd;
	}else{
		return -1;
	}
}

void ab_delete_address(struct address_book* b, const char* key, size_t sz){
	if(b == NULL || key == NULL){
		return;
	}
	char k[MAX_ADDRESS] = {'\0'};
	size_t s = (sz > (MAX_ADDRESS -1))? MAX_ADDRESS - 1 : sz;
	memcpy(k, key, s);
	struct address* addr = b->entries[hash(k) % MAX_ENTRY];
	if(addr == NULL){
		return;
	}

	if(strcmp(addr->key, k) == 0){
		b->entries[hash(k) % MAX_ENTRY] = NULL;
		free(addr);
		return;
	}
	while(addr->next){
		if(strcmp(addr->next->key, k) == 0){
			struct address* tmp = addr->next->next;
			free(addr->next);
			addr->next = addr->next->next;
			return;
		}
		addr = addr->next;
	}
}

char* test(char addr[], int* port){
	char* a = strtok(addr, ":");
	if(a != NULL){
		char* b = strtok(NULL, ":");
		*port = atoi(b);
	}
	return a;
}

/*
int main(){
	struct address_book* b = address_book_new();
	ab_add_address(b, "127.0.0.1:1234", sizeof("127.0.0.1:1234"), 12);
	struct address* a = ab_find_address(b, "127.0.0.1:1234", sizeof("127.0.0.1:1234"));
	printf("%i\n", a->fd);
	//ab_delete_address(b, "helloworld", strlen("helloworld"));
	a = ab_find_address(b, "helloworld", sizeof("helloworld"));
	if(a){
		printf("error\n");
	}
	address_book_delete(&b);
}
*/

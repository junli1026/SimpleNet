#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "buffer.h"

struct buffer{
	int capacity;
	int size;
	int idx;
	char* data[2];
};

/// Round up to next higher power of 2 (return x if it's already a power of 2).
static inline int pow2roundup(int x){
    if (x < 0){
        return 0;
	}
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

struct buffer* buffer_new(int capacity){
	if(capacity <= 0){
		return NULL;
	}

	struct buffer* buf = (struct buffer*)malloc(sizeof(*buf));
	if(buf){
		buf->size = 0;
		buf->idx = 0;
		capacity = pow2roundup(capacity);
		buf->capacity = capacity;
		buf->data[0] = (char*)calloc(capacity, sizeof(char));
		if(buf->data[0] == NULL){
			free(buf);
			return NULL;
		}
		buf->data[1] = (char*)calloc(capacity, sizeof(char));
		if(buf->data[1] == NULL){
			free(buf->data[0]);
			free(buf);
			return NULL;
		}
	}
	return buf;
}

void buffer_delete(struct buffer** b_addr){
	assert(b_addr);
	struct buffer* b = *b_addr;
	if(b){
		free(b->data[0]);
		free(b->data[1]);
		free(b);
	}
	*b_addr = NULL;
}

static int expand(struct buffer *buf){
	int c = buf->capacity * 2;
	char* tmp1 = (char*)calloc(c, sizeof(char));
	if(tmp1 == NULL){
		return -1;
	}

	char* tmp2 = (char*)realloc(buf->data[buf->idx], c);
	if(tmp2== NULL){
		free(tmp1);
		return -1;
	}

	int i = (buf->idx + 1) %2 ;
	free(buf->data[i]);
	buf->data[buf->idx] = tmp2;
	buf->data[i] = tmp1;
	buf->capacity = c;
	return 0;
}

void* buffer_data(struct buffer* buf, size_t* sz){
	if(buf){
		*sz = buf->size;
		return buf->data[buf->idx];
	}else{
		*sz = 0;
		return NULL;
	}
}

int buffer_append(struct buffer* buf, void* data, size_t len){
	if(buf == NULL || data == NULL || len == 0){
		return -1;
	}

	while(buf->size + len >= buf->capacity){
		if(expand(buf) < 0){
			return -1;
		}
	}
	char* dst = buf->data[buf->idx] + buf->size;
	memcpy(dst, data, len);
	buf->size += len;
	return 0;
}

void buffer_truncate(struct buffer* buf, size_t n){
	if(buf){
		if(n >= buf->size){
			buf->size = 0;
		}else{
			char* src = buf->data[buf->idx] + n;
			int i = (buf->idx + 1) % 2;
			memcpy(buf->data[i], src, buf->size - n);
			buf->idx = i;
			buf->size -= n;
		}
	}
}

void buffer_clear(struct buffer* buf){
	if(buf){
		buf->size = 0;
	}
}

/*
int main(){
	struct buffer* b = buffer_new(1024);
	buffer_delete(&b);
}
*/
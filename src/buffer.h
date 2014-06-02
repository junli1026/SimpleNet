#ifndef _BUFFER_H_
#define _BUFFER_H_

struct buffer;

struct buffer* buffer_new(int capacity);

void buffer_delete(struct buffer** buffer_addr);

void* buffer_data(struct buffer* buf, size_t* sz);

int buffer_append(struct buffer* buf, void* data, size_t len);

void buffer_truncate(struct buffer* buf, size_t n);

void buffer_clear(struct buffer* buf);

#endif
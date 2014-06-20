#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string.h>
#include <stdint.h>
#include <vector>

#include "message.h"

namespace simple{

const int DefaultBufferSize = 64;
const int MaxEmptyHead = 1024;

class Buffer{
private:
	std::vector<uint8_t> b_;
	int rIndex_;
	int wIndex_;
	void removeEmptyHead();
	void expand(size_t);

	Buffer& operator=(const Buffer& rhs){}
	Buffer(const Buffer&){}

public:
	Buffer();
	~Buffer();

	void clear();
	void truncate(size_t sz);
	size_t size() const;
	const uint8_t* begin() const;
	std::vector<uint8_t> retrieveBy(const void* separator, size_t sz);

	void append(const void* src, size_t len);
	void append(const std::vector<uint8_t>& v);
	void append(const Message& m);
};

}

#endif
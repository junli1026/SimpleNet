#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string.h>
#include <stdint.h>
#include <vector>

namespace simple{
class Buffer
{
private:
	std::vector<uint8_t> b_;
	int rIndex_;
	int wIndex_;

	void moveHead();
	//non-copyable
	Buffer& operator=(const Buffer& rhs) const{}
	Buffer(const Buffer&){}
public:
	Buffer();
	~Buffer();
	size_t size();
	void clear();
	void append(const void* src, size_t len);
	uint8_t* begin();
	void truncate(size_t sz);
	std::vector<uint8_t> retrieveBy(const void* separator, size_t sz);
};

}

#endif
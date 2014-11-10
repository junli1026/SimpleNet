#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string.h>
#include <stdint.h>
#include <vector>
#include <memory>
#include "block.h"

namespace simple{

const int InitBufferSize = 64;
const int MaxDummySize = 1024;

//this class does not provide any method to cut package; user can use CRLF as seperator or other mechanism to handle that
class Buffer{
private:
	std::vector<uint8_t> b_;
	int rIndex_;
	int wIndex_;

	uint8_t* begin();
	void expand(size_t);
	void tryClearDummy();
	void moveReadIndex(size_t sz);
	
	Buffer& operator=(const Buffer& rhs){}
	Buffer(const Buffer&){}
public:
	Buffer();
	~Buffer();

	void clear();
	bool empty();
	size_t size() const;
	
	//return the data before first seperator, seperator is not included in the returned block;
	std::shared_ptr<Block> retrieveBy(const void* separator, size_t sz); 
	
	//return fixed sized block, if buffer size < sz, all data is retrieved
	std::shared_ptr<Block> retrieve(size_t sz);

	//retrieve a string
	std::string retrieveString();

	//return the first integer, if buffer size < sizeof(int), 0 is returned and nothing happened to buffer
	int retrieveInt();
	
	void append(const void* src, size_t len);
	void append(const std::vector<uint8_t>& v);
	void append(const Block& b);
};

}

#endif

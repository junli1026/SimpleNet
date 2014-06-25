#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <stdint.h>
#include <string>
#include <memory>

namespace simple{

class Block{
private:
	uint8_t* data_;
	size_t sz_;

	void copyFrom(const void* src, size_t sz);
	void initFrom(const void* src, size_t sz);
	Block(){}

public:
	Block(const Block& b);
	Block(const char* str);
	Block(const std::string& str);
	Block(const std::shared_ptr<Block>& b);
	Block(const void* src, size_t sz);
	~Block();

	Block& operator=(const Block& rhs);
	bool operator==(const Block& rhs) const;
	bool operator!=(const Block& rhs) const;

	const uint8_t* begin() const;
	size_t size() const;
	bool empty() const;

	bool startWith(const void* src, size_t sz) const;
	bool endWith(const void* src, size_t sz) const;
	std::string dump2String();
};
}
#endif


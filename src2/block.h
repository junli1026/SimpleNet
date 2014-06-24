#include <stdint.h>
#include <string>

namespace simple{
class Block
{
private:
	uint8_t* data_;
	size_t sz_;

	void copyFrom(const void* src, size_t sz);
	void initFrom(const void* src, size_t sz);
	Block(){}

public:
	Block(const void* src, size_t sz);
	explicit Block(const std::string& str);
	explicit Block(const char* str);
	explicit Block(const Block& b);
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


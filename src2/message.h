#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace simple{
class Message{
private:
	std::vector<uint8_t> v_;

	void copyFrom(const Message& m);
	void initFrom(const void* src, size_t t);
public:
	Message(const void* src, size_t sz);
	Message(const std::string str);
	Message(const Message& m);
	Message(const char* str);
	Message();
	
	Message& operator=(const Message& m);
	bool operator==(const Message& rhs) const;
	bool operator!=(const Message& rhs) const;

	const uint8_t* begin() const;
	size_t size() const;
	bool empty() const;
};
}
#endif

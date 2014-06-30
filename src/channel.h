#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <vector>
#include <queue>
#include <string>
#include <stdint.h>
#include "buffer.h"
#include "block.h"
#include "event.h"

namespace simple{

/*
class Message{
public:
	std::string header;
	std::shared_ptr<Block> body;
	Message(std::string h, std::shared_ptr<Block> b){
		this->header = h;
		this->body = b;
	}
};
*/

class Channel{
private:
	int rfd_;
	int wfd_;
	int epfd_;
	Buffer buf_;
	std::queue<std::string> qheader_;
	std::queue<std::shared_ptr<Block>> qbody_;

	Channel& operator=(const Channel& ch){}
	Channel(const Channel& ch){}
	bool hasData();
	void writeHeader(const std::string& header);
public:
	Channel();
	~Channel();
	bool hasEvent();
	void addEvent(const std::string& str); //write a command without data
	void addEvent(const std::string& str, const void* data, size_t sz);
	void addEvent(const std::string& str, std::shared_ptr<Block> data);
	Event<std::shared_ptr<Block>> nextEvent();
};

}
#endif

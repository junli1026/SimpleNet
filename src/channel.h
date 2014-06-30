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
	void addPublishEvent(std::shared_ptr<Block> data);
	void addListenEvent(std::shared_ptr<Block> data);
	void addConnectEvent(std::shared_ptr<Block> data);
	void addExitEvent();
	
	void addEvent(const std::string& str); //write a command without data
	void addEvent(const std::string& str, const void* data, size_t sz);
	void addEvent(const std::string& str, std::shared_ptr<Block> data);
	
	bool hasEvent();
	Event<std::shared_ptr<Block>> nextEvent();
};

}
#endif

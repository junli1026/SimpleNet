#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <vector>
#include <queue>
#include <string>
#include <stdint.h>
#include "buffer.h"
#include "block.h"

namespace simple{

class Channel{
private:
	int rfd_;
	int wfd_;
	int epfd_;
	Buffer buf_;
	std::queue<std::string> q_;
	
	Channel& operator=(const Channel& ch){}
	Channel(const Channel& ch){}
	bool hasData();
public:
	Channel();
	~Channel();
	bool hasMessage();
	void writeMessage(const std::string& str);
	std::string nextMessage();

};

}
#endif

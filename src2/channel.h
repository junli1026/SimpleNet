#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <vector>
#include <queue>
#include <string>
#include <stdint.h>
#include "buffer.h"

namespace simple{

typedef std::vector<std::string> ControlWords;

class Channel{
private:
	int rfd_;
	int wfd_;
	int epfd_;
	Buffer buf_;
	std::queue<ControlWords> q_;
	
	Channel& operator=(const Channel& ch);
	Channel(const Channel& ch);
	bool hasData();
public:
	Channel();
	~Channel();
	bool hasControlWords();
	void writeControlWords(const ControlWords& words);
	ControlWords getControlWords();

};

}
#endif
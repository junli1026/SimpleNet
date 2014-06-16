#ifndef _CHANNEL_H_
#define _CHANNEL_H_

namespace simple{

class Channel{
private:
	int rfd_;
	int wfd_;
	int epfd_;
	bool healthy_;
public:
	Channel();
	~Channel();
	bool HasData();
	bool Healthy();
	int Readfd();
	int Writefd();
};

}
#endif
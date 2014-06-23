#ifndef _EVNET_LOOP_H_
#define _EVNET_LOOP_H_

namespace simple{
class EventLoop{

private:
	Channel channel_;
public:
	EventLoop(Poller& p, Connector& c, Acceptor& a, IOHandler& h);

	
	void loop();
};

}
#endif
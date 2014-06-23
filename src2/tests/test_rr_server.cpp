#include "../request_response/rr_server.h"
#include "../message.h"
#include <functional>
#include <string>

using namespace simple;
int main(){

	RRServer s;
	s.monitor("127.0.0.1", 8001);
	s.onRequest([](Message m){
		std::string req(m.begin(), m.begin() + m.size());
		return Message(req);
	});
	while(true){
		s.run();
	}
}
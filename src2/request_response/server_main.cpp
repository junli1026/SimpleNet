#include "rr_server.cpp"

using namespace simple;
int main(){

	RRServer s;
	s.monitor("127.0.0.1", 8001);
	while(true){
		s.run();
	}
}
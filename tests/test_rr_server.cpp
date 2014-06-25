#include "../src/request_response/rr_server.h"
#include "../src/block.h"
#include <functional>
#include <memory>
#include <string>
#include <iostream>

using namespace simple;
int main(){

	RRServer s;
	s.monitor("127.0.0.1", 8001);
	
	s.onRequest([](std::shared_ptr<Block> b){
		return Block(b);
	});

	
	while(true){
		s.run();
	}
}
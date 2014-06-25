#include "../src/publish_subscribe/publisher.h"

#include <string>
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <string>

using namespace simple;


int main(){
	Publisher p("127.0.0.1", 8001);
	p.runService();

	std::string a;
	while(true){
		std::getline(std::cin, a);
		if(a == std::string("quit")){
			break;
		}else{
			p.publish(a.c_str(), strlen(a.c_str()));
		}
	}
	p.stopService();
	sleep(1);
}

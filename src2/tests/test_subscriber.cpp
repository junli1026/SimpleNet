#include "../publish_subscribe/subscriber.h"

#include <iostream>
#include <stdint.h>
using namespace std;

int main(){
	simple::Subscriber s;
	s.subscribe("127.0.0.1", 8001);
	
	std::vector<uint8_t> v = s.receiveData();

	for(auto ch : v){
		cout << static_cast<char>(ch);
	}
	cout << endl;
}

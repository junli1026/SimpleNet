#include "rr_client.h"

#include <iostream>
#include <stdint.h>
using namespace std;

int main(){
	simple::RRClient c;
	c.doConnect("127.0.0.1", 8001);
	std::cout << "here" << endl;
	c.sendData("hello", strlen("hello"), false);
	c.sendData("world", strlen("world"), true);

	std::vector<uint8_t> v = c.receiveData();

	for(auto ch : v){
		cout << static_cast<char>(ch);
	}
	cout << endl;
}
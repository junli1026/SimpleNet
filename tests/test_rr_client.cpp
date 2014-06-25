#include "../src/request_response/rr_client.h"

#include <iostream>
#include <stdint.h>
#include <string>
using namespace std;

int main(){
	simple::RRClient c;
	c.doConnect("127.0.0.1", 8001);
	
	std::string a;
	const char* str;
	std::vector<uint8_t> v;
	while(true){
		std::getline(std::cin, a);
		str = a.c_str();
		c.sendData(str, strlen(str), true);
		v = c.receiveData();
		for(auto ch : v){
			cout << static_cast<char>(ch);
		}
		cout << endl;
	}
}
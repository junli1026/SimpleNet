//#include "socket.h"
//#include "acceptor.h"
//#include "reader.h"
//#include "writer.h"
#include <iostream>
#include <memory>
#include <string.h>
#include <vector>

int main(){
	/*
	simple::Acceptor acceptor;
	simple::ASocket s(12);
	simple::Context ctx = acceptor.doAccept(&s);
	std::cout << ctx.connfd;
	std::cout << ctx.operation;

	std::vector<char> v;
	v.push_back('1');
	v.push_back('2');
	v.push_back('3');
	v.push_back('4');
	v.push_back('5');


	char start[3];
	memcpy(start, &v[0], 3);
	std::cout << std::endl;
	for(int i = 0; i < 3; i++){
		std::cout << start[i] << std::endl;
	}
*/
	std::shared_ptr<int> p; //= std::make_shared<int>(12);
	if(!p){
		std::cout << "is null";
	}

}
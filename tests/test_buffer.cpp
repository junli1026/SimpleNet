#include "../src/buffer.h"
#include "../src/block.h"
#include <assert.h>
#include <string>
#include <iostream>

using namespace simple;
int main(){
	Buffer b;
	int a[3] = {12, 13, 14};
	b.append(a, sizeof(a));
	assert(b.retrieveInt() == 12);
	assert(b.size() == sizeof(int) * 2);
	assert(b.retrieveInt() == 13);
	assert(b.size() == sizeof(int) * 1);
	assert(b.retrieveInt() == 14);
	assert(b.size() == sizeof(int) * 0);
	assert(b.retrieveInt() == 0);	

	b.append(Block("hello"));
	b.append("\r\n", 2);
	auto ret = b.retrieveBy("\r\n", 2);
	assert(b.size() == 0);
	assert(ret->size() == strlen("hello") + 1);
	std::string str = ret->dump2String();
	assert(str == std::string("hello"));
}

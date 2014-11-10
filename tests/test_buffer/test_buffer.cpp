#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>
#include <assert.h>
#include "../../src/buffer.h"
#include "../../src/block.h"

using namespace simple;

void test_buffer_1(std::shared_ptr<Buffer> buffer){
	const char* str = "hello world";
	buffer->append(str, strlen(str)+1);
	assert(buffer->size() == strlen(str) + 1);
	std::string s = buffer->retrieveString();
	assert(s == std::string(str));
	assert(buffer->empty());
	assert(buffer->size() == 0);
}

void test_buffer_2(std::shared_ptr<Buffer> buffer){
	int arr[] = {12, 13, 14, 16, 15};
	buffer->append(arr, sizeof(arr));
	assert(buffer->size() == sizeof(arr));
	for(int i = 0; i < (int) sizeof(arr) / (int) sizeof(int); i++){
		assert(buffer->size() == sizeof(arr) - i*sizeof(int));
		int integer = buffer->retrieveInt();
		assert(integer == arr[i]);
	}
	assert(buffer->empty());
	assert(buffer->size() == 0);
}

void test_buffer_3(std::shared_ptr<Buffer> buffer){
	std::vector<uint8_t> v;
	const char* str = "hello world";
	for(int i = 0; i < (int)strlen(str); i++){
		v.push_back(str[i]);
	}
	const char* sep = "ld";
	buffer->append(v);
	std::shared_ptr<Block> b = buffer->retrieveBy(sep, strlen(sep));
	Block res("hello wor", strlen("hello wor"));
	assert(res == b);
}

void test_buffer(){
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
	test_buffer_1(buffer);
	test_buffer_2(buffer);
	test_buffer_3(buffer);
	std::cout << "test buffer passed" << std::endl;
}

int main(){
	test_buffer();
	return 0;
}



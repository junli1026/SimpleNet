#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

#include <iostream>

namespace simple{

class Request{
private:
	std::vector<uint8_t> data_;
	inline void copyFrom(const Request& req);

	Request();
public:
	Request(const char* str);
	Request(const void* src, size_t sz);

	Request(Request& req);
	Request& operator=(Request& req);

	size_t size() const;

	const uint8_t* begin() const;

	~Request();

	std::string toString();
};


const uint8_t* Request::begin() const{
	return &this->data_[0];
}

size_t Request::size() const{
	return this->data_.size();
}

inline void Request::copyFrom(const Request& req){
	if(this->data_.capacity() > req.size()){
		this->data_.resize(req.size());
	}else{
		this->data_.reserve(req.size());
	}
	const uint8_t* b = req.begin();
	std::copy(b, b+ req.size(), data_.begin());
}

Request::Request(const char* str){
	data_.reserve(strlen(str) + 3);
	this->data_.insert(this->data_.begin(), str, str + strlen(str) + 1);
	data_.push_back(static_cast<uint8_t>('\r'));
	data_.push_back(static_cast<uint8_t>('\n'));
}

Request::Request(const void* src, size_t sz){
	const uint8_t* s = static_cast<const uint8_t*>(src);
	this->data_.reserve(sz + 2);
	this->data_.insert(this->data_.begin(), s, s + sz);
	this->data_.push_back(static_cast<uint8_t>('\r'));
	this->data_.push_back(static_cast<uint8_t>('\n'));
}

Request::Request(Request& req){
	copyFrom(req);
}

Request& Request::operator=(Request& req){
	copyFrom(req);
	return *this;
}

Request::~Request(){

}

std::string Request::toString(){
	std::string ret (this->data_.begin(), this->data_.end());
	return ret;
}

}

int main(int argc, char const *argv[])
{
	simple::Request a("hello");
	simple::Request b("thisa");
	b = a;

	std::cout << b.toString();
	return 0;
}

#endif
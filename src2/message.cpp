#include "message.h"

#include <string.h>
#include <algorithm>
#include <vector>
#include <assert.h>

namespace simple{

Message::Message(){}

void Message::copyFrom(const Message& m){
	if(this->v_.capacity() < m.size()){
		this->v_.reserve(m.size());	
	}
	this->v_.resize(m.size());
	std::copy(m.begin(), m.begin() + m.size(), this->v_.begin());
}

void Message::initFrom(const void* src, size_t sz){
	this->v_.reserve(sz);
	const uint8_t* s = static_cast<const uint8_t*>(src);
	std::copy(s, s + sz, this->v_.begin());
}

Message::Message(const void* src, size_t sz){
	this->initFrom(src, sz);
}

Message::Message(const std::string str){
	const char* s = str.c_str();
	this->initFrom(s, strlen(s) + 1);
}

Message::Message(const char* str){
	this->initFrom(str, strlen(str) + 1);
}

Message::Message(const Message& m){
	this->copyFrom(m);
}
	
Message& Message::operator=(const Message& m){
	this->copyFrom(m);
}

bool Message::operator ==(const Message& rhs) const{
	if(this->size() != rhs.size()){
		return false;
	}
	const uint8_t* l = this->begin();
	const uint8_t* r = rhs.begin();
	for(int i = 0; i < rhs.size(); i++){
		if(l[i] != r[i]){
			return false;
		}
	}
	return true;
}

bool Message::operator!=(const Message& rhs) const{
	return !(*this == rhs);
}

const uint8_t* Message::begin() const{
	return &this->v_[0];
}

size_t Message::size() const{
	return this->v_.size();
}

}

/*
int main(){
	using namespace simple;
	Message a("hello");
	Message b(std::string("hello"));
	assert(a == b);
		
	Message c = a;

	assert(c == b);
}
*/
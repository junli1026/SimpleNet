#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include "buffer.h"

namespace simple{

Buffer::Buffer(){
	this->rIndex_ = 0;
	this->wIndex_ = 0;
	this->b_.resize(InitBufferSize);
}

Buffer::~Buffer(){
}

void Buffer::expand(size_t try_append_size){
	while(this->b_.size() < this->wIndex_ + try_append_size){
		if(this->b_.size() == 0){
			this->b_.resize(InitBufferSize);
		}else{
			this->b_.resize(this->b_.size() * 2);
		}
	}
}

size_t Buffer::size() const{
	return this->wIndex_ - this->rIndex_;
}

bool Buffer::empty(){
	return this->size() == 0;
}

void Buffer::moveReadIndex(size_t sz){
	if(sz >= this->size()){
		this->clear();
	}else{
		this->rIndex_ += sz;
	}
	this->tryClearDummy();
}

uint8_t* Buffer::begin(){
	auto first = this->b_.begin() + this->rIndex_;
	return &(*first);
}

//try to clear unused/dummy data before rIndex_
void Buffer::tryClearDummy(){
	if(this->rIndex_  < MaxDummySize){ //not need to move
		return;
	}

	if(this->rIndex_ == 0){
		return;
	}

	if(this->empty()){
		this->rIndex_ = 0;
		this->wIndex_ = 0;
		return;
	}
	int sz = this->size();
	std::vector<uint8_t> tmp(this->b_.begin() + this->rIndex_, this->b_.begin() + this->wIndex_);
	std::copy(tmp.begin(), tmp.end(), this->b_.begin());
	this->rIndex_ = 0;
	this->wIndex_ = sz;
}

void Buffer::append(const void* src, size_t len){
	expand(len);
	const uint8_t* s = static_cast<const uint8_t*>(src);
	std::copy(s, s+len, this->b_.begin() + this->wIndex_);
	this->wIndex_ += len;
}

void Buffer::append(const std::vector<uint8_t>& v){
	expand(v.size());
	std::copy(v.begin(), v.end(), this->b_.begin() + this->wIndex_);
	this->wIndex_ += v.size();
}

void Buffer::append(const Block& b){
	expand(b.size());
	std::copy(b.begin(), b.begin() + b.size(), this->b_.begin() + this->wIndex_);
	this->wIndex_ += b.size();
}

void Buffer::clear(){
	this->rIndex_ = 0;
	this->wIndex_ = 0;
	this->b_.clear();
}

std::shared_ptr<Block> Buffer::retrieve(size_t sz){
	if(sz == 0){
		return nullptr;
	}

	if(this->empty()){
		return nullptr;
	}

	size_t rsz = this->size() > sz ? sz : this->size();
	auto ret = std::make_shared<Block>(this->begin(), rsz);
	this->moveReadIndex(rsz);
	return ret;
}

std::shared_ptr<Block> Buffer::retrieveBy(const void* separator, size_t sz){
	if(separator == NULL || sz == 0){
		return nullptr;
	}
	
	if(this->empty()){
		return nullptr;
	}

	auto first = this->b_.begin() + this->rIndex_;
	auto last = this->b_.begin() + this->wIndex_;
	const uint8_t* s = static_cast<const uint8_t*>(separator);
	auto it = std::search(first, last, s, s+sz); 
	if(it == last){ //seperator not found
		return nullptr;
	}
	
	size_t dsz = it - first; //data size
	size_t rsz = it - first + sz; // readable size
	auto ret = std::make_shared<Block>(this->begin(), dsz);
	this->moveReadIndex(rsz);
	return ret;
} 

int Buffer::retrieveInt(){
	if(this->size() < sizeof(int)){
		return -1;
	}else{
		int ret = *(int*)this->begin();
		this->moveReadIndex(sizeof(int));
		return ret;
	}
}

std::string Buffer::retrieveString(){
	if(this->empty()){
		return std::string();
	}

	auto first = this->b_.begin() + this->rIndex_;
	auto last = this->b_.begin() + this->wIndex_;
	char s = '\0';
	auto it = std::find(first, last, (uint8_t)s);
	if(it == last){ //'\0' not found
		return std::string();
	}
	
	size_t rsz = it - first + sizeof(char); // readable size
	std::string ret(first, it);
	this->moveReadIndex(rsz);
	return ret;
}

}

/*
int main(){
	simple::Buffer b;
	b.append("hello", strlen("hello"));
	b.append("\r\n",2);
	b.append("this is a test", strlen("this is a test"));
	b.append("\r\n",2);
	
	auto tmp = b.retrieveBy("\r\n", 2);
	for(auto c : tmp){
		std::cout << c;
	}
	std::cout << std::endl;

	tmp = b.retrieveBy("\r\n", 2);
	for(auto c : tmp){
		std::cout << c;
	}
	std::cout << std::endl;
}
*/

#include <algorithm>
#include <vector>
#include <iostream>
#include <stdint.h>
#include "buffer.h"

namespace simple{

Buffer::Buffer(){
	this->rIndex_ = 0;
	this->wIndex_ = 0;
	this->b_.resize(DefaultBufferSize);
}

Buffer::~Buffer(){
}

void Buffer::expand(size_t try_append_size){
	while(this->b_.size() < this->wIndex_ + try_append_size){
		if(this->b_.size() == 0){
			this->b_.resize(DefaultBufferSize);
		}else{
			this->b_.resize(this->b_.size() * 2);
		}
	}
}

void Buffer::removeEmptyHead(){
	if(this->rIndex_ == 0){
		return;
	}
	if(this->rIndex_ == this->wIndex_){
		this->rIndex_ = 0;
		this->wIndex_ = 0;
		return;
	}
	std::vector<uint8_t> tmp(this->b_.begin() + this->rIndex_, this->b_.begin() + this->wIndex_);
	std::copy(tmp.begin(), tmp.end(), this->b_.begin());
	this->rIndex_ = 0;
	this->wIndex_ = tmp.end() - tmp.begin();
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
}

const uint8_t* Buffer::begin() const{
	if(this->size() > 0){
		return &this->b_[this->rIndex_];
	}else{
		return NULL;
	}
}

void Buffer::truncate(size_t sz){
	if(sz >= this->size()){
		this->clear();
	}else{
		this->rIndex_ += sz;
	}
}

size_t Buffer::size() const{
	return this->wIndex_ - this->rIndex_;
}

std::shared_ptr<Block> Buffer::retrieveBy(const void* separator, size_t sz){
	if(separator == NULL || sz == 0){
		return nullptr;
	}
	
	auto first = this->b_.begin() + this->rIndex_;
	auto last = this->b_.begin() + this->wIndex_;
	if(first == last){
		return nullptr;
	}

	const uint8_t* s = static_cast<const uint8_t*>(separator);
	auto it = std::search(first, last, s, s+sz); 
	if(it == last){
		return nullptr;
	}
	
	size_t bsz = it - first;
	if(bsz == 0){
		this->rIndex_ += sz;
		return nullptr;
	}
	
	auto ret = std::make_shared<Block>(&this->b_[this->rIndex_], bsz);
	this->rIndex_ += it - first + sz;
	
	if(this->rIndex_  > MaxEmptyHead){
		removeEmptyHead();
	}
	return ret;
} 

int Buffer::retrieveInt(){
	if(this->size() < sizeof(int)){
		return 0;
	}else{
		int ret = *(int*)this->begin();
		this->truncate(sizeof(int));
		return ret;
	}
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

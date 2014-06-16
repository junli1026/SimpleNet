#include <algorithm>
#include <vector>
#include <stdint.h>
#include "buffer.h"

namespace simple{

Buffer::Buffer()
{
	this->rIndex_ = 0;
	this->wIndex_ = 0;
	this->b_.resize(512);
}

Buffer::~Buffer()
{
}

void Buffer::moveHead(){
	if(this->rIndex_ == 0){
		return;
	}
	if(this->rIndex_ == this->wIndex_){
		this->rIndex_ = 0;
		this->wIndex_ = 0;
	}
	std::vector<uint8_t> tmp(this->b_.begin() + this->rIndex_, this->b_.begin() + this->wIndex_);
	std::copy(tmp.begin(), tmp.end(), this->b_.begin());
	this->rIndex_ = 0;
	this->wIndex_ = tmp.end() - tmp.begin();
}

void Buffer::append(const void* src, size_t len){
	while(this->b_.size() < this->wIndex_ + len){
		this->b_.resize(this->b_.size() * 2);
	}
	const uint8_t* s = static_cast<const uint8_t*>(src);
	std::copy(s, s+len, this->b_.begin() + this->wIndex_);
	this->wIndex_ += len;
}

void Buffer::clear(){
	this->b_.clear();
	this->rIndex_ = 0;
	this->wIndex_ = 0;
}

uint8_t* Buffer::begin(){
	if(this->b_.size() > 0){
		return &this->b_[0];
	}else{
		return NULL;
	}
}

void Buffer::truncate(size_t sz){
	if(sz > this->size()){
		this->clear();
	}else{
		this->rIndex_ += sz;
	}
}

size_t Buffer::size(){
	return this->wIndex_ - this->rIndex_;
}

std::vector<uint8_t> Buffer::retrieveBy(const void* separator, size_t sz){
	auto first = this->b_.begin() + this->rIndex_;
	auto last = this->b_.begin() + this->wIndex_;
	std::vector<uint8_t> ret;
	if(first == last){
		return ret;
	}else{
		const uint8_t* s = static_cast<const uint8_t*>(separator);
		if(s == NULL){
			return ret;
		}
		auto it = std::search(first, last, s, s+sz); 
		if(it == last){
			return ret;
		}
		ret.resize(it + sz - first);
		std::copy(first, it + sz, ret.begin());
		this->rIndex_ += it - first + sz;
	}

	if(this->rIndex_  > 1024){
		moveHead();
	}
	return ret;
} 

}

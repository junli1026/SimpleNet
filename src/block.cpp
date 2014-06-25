#include "block.h"
#include <string.h>
#include <stdio.h>

namespace simple{
Block::~Block(){
	delete[] this->data_;
}

void Block::copyFrom(const void* src, size_t sz){
	this->sz_ = 0;
	delete[] this->data_;
	this->initFrom(src, sz);
}

void Block::initFrom(const void* src, size_t sz){
	if(src == NULL || sz == 0){
		return;
	}
	const uint8_t* s = static_cast<const uint8_t*>(src);
	this->data_ = new uint8_t[sz];
	memcpy(this->data_, s, sz);
	this->sz_ = sz;
}

Block::Block(const void* src, size_t sz){
	initFrom(src, sz);
}

Block::Block(const std::string& str) {
	initFrom(str.c_str(), str.size() + 1);
}

Block::Block(const char* str){
	initFrom(str, strlen(str) + 1);
}

Block::Block(const Block& b){
	copyFrom(b.begin(), b.size());
}

Block::Block(const std::shared_ptr<Block> &b){
	copyFrom(b->begin(), b->size());
}

Block& Block::operator=(const Block& rhs){
	copyFrom(rhs.begin(), rhs.size());
	return *this;
}

bool Block::operator==(const Block& rhs) const{
	if(this->size() != rhs.size()){
		return false;
	}
	return (memcmp(this->begin(), rhs.begin(), this->size()) == 0);
}

bool Block::operator!=(const Block& rhs) const{
	return !((*this) == rhs);
}

const uint8_t* Block::begin() const{
	return this->data_;
}

size_t Block::size() const{
	return this->sz_;
}

bool Block::empty() const{
	return (this->sz_ == 0);
}

bool Block::startWith(const void* src, size_t sz) const{
	if(this->size() < sz || src == NULL){
		return false;
	}
	return (memcmp(this->begin(), src, sz) == 0);
}

bool Block::endWith(const void* src, size_t sz) const{
	if(this->size() < sz || src == NULL){
		return false;
	}
	return (memcmp(this->begin() + this->size() - sz, src, sz) == 0);
}

std::string Block::dump2String(){
	if(this->sz_ == 0){
		return std::string();
	}
	if(this->data_[this->sz_ -1] != '\0'){
		return std::string();
	} else{
		return std::string(this->begin(), this->begin() + this->size()-1);
	}
}

}

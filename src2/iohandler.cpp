
#include "iohandler.h"
#include <iostream>

namespace simple{

IOHandler::IOHandler(){
}

IOHandler::~IOHandler(){
}

bool IOHandler::contains(int fd){
	auto it = this->sockets_.find(fd);
	return (it != this->sockets_.end());
}

void IOHandler::add(int fd){
	if(this->contains(fd)){
		//to do:: throw exception
	}else{
		auto s = std::make_shared<IOSocket>(fd);
		s->setNonBlock();
		this->sockets_.insert(std::pair<int, std::shared_ptr<IOSocket>>(fd, s));
	}
}

void IOHandler::erase(int fd){
	this->sockets_.erase(fd);
}

void IOHandler::doRead(int fd){
	if(this->contains(fd)){
		//throw exception
	}
	auto s = this->sockets_.find(fd)->second;
	auto buf = s->readBuffer();
	uint8_t b[1024];
	int n = read(fd, b, 1024);
	if(n < 0){
		//throw exception
	}else if(n == 0){
		s->setClose();
	}else{
		buf->append(b, n);
	}
}

void IOHandler::doWrite(int fd){	
	auto s = this->sockets_.find(fd)->second;
	auto buf = s->writeBuffer();
	if(buf->size() == 0){
		std::cout << "buffer size " << std::endl;
		return;
	}
	size_t sz = 1024;
	if(buf->size() < sz){
		sz = buf->size();
	}
	int n = write(fd, buf->begin(), sz);

	if(n < 0){
		//to do: throw exception
	}else if(n == 0){
		//to do throw exception
	}else{
		buf->truncate(n);
	}
}

std::shared_ptr<Buffer> IOHandler::getReadBuffer(int fd){
	if(this->contains(fd)){
		return this->sockets_.find(fd)->second->readBuffer();
	}else{
		return nullptr;
	}
}

std::shared_ptr<Buffer> IOHandler::getWriteBuffer(int fd){
	if(this->contains(fd)){
		return this->sockets_.find(fd)->second->writeBuffer();
	}else{
		return nullptr;
	}
}

std::map<int, std::shared_ptr<IOSocket>>& IOHandler::getSocketMap(){
	return this->sockets_;	
}

}

#ifndef _EVENT_H_
#define _EVENT_H_
#include <string>

namespace simple{

const std::string GL_EventNone = std::string();
const std::string GL_EventRead = std::string("read"); 
const std::string GL_EventWrite = std::string("write");
const std::string GL_EventAccept = std::string("accept");
const std::string GL_EventExit = std::string("exit");

template<typename T>
class Event{
private:
	T data_;
	std::string type_;
public:
	Event(std::string t, T d){
		this->data_ = d;
		this->type_ = t;
	}
	
	std::string type(){
		return this->type_;
	}

	T data(){
		return this->data_;
	}
};

}
#endif

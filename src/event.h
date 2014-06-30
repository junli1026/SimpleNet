#ifndef _EVENT_H_
#define _EVENT_H_
#include <string>

namespace simple{

const int EventNone	= 0;
const int EventAccept	= 1;
const int EventConnect	= 2;
const int EventRead	= 3;
const int EventWrite	= 4
const int EventExit	= 5;
const int EventPublish	= 6;


template<typename T>
class Event{
private:
	T data_;
	int type_;
public:
	Event(int t, T d){
		this->data_ = d;
		this->type_ = t;
	}

	Event(const Event& rhs){
		this->data_ = rhs.data();
		this->type_ = rhs.type();
	}
	
	Event& operator=(const Event& rhs){
		this->data_ = rhs.data();
		this->type_ = rhs.type();
	}
	
	int type(){
		return this->type_;
	}

	T data(){
		return this->data_;
	}
};

}
#endif

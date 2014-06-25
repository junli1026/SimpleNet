#ifndef _SAFE_QUEUE_H_
#define _SAFE_QUEUE_H_

#include <queue>
#include "../mutex_lock.h"

namespace simple{

template<typename T>
class SafeQueue{
private:
	MutexLock lock_;
	std::queue<T> q_;
	T emptyValue_;

	SafeQueue& operator=(const SafeQueue& rhs){}
	SafeQueue(const SafeQueue& rhs){}
	SafeQueue(){}
public:
	SafeQueue(T emptyValue){
		this->emptyValue_ = emptyValue;
	}

	T dequeue(){
		this->lock_.lock();
		if(this->q_.empty()){
			T ret = emptyValue_;
		}else{
			T ret = this->q_.pop();
		}
		this->lock_.unlock();
		return ret;
	}

	void enqueue(T v){
		this->lock_.lock();
		this->q_.push(v);
		this->lock_.unlock();
	}

	bool empty(){
		this->lock_.lock();
		bool ret = this->q_.empty();
		this->lock_.unlock();
		return ret;
	}
};
}

#endif
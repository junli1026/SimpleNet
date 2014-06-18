#ifndef _MUTEX_LOCK_H_
#define _MUTEX_LOCK_H_

#include <pthread.h>
#include <assert.h>

namespace simple{

class MutexLock{
private:
	pthread_mutex_t mutex_;

	MutexLock(const MutexLock& m){}
	MutexLock& operator=(const MutexLock& m){}
public:
	MutexLock(){
		int err = pthread_mutex_init(&this->mutex_, NULL);
		assert(err == 0);
	}

	~MutexLock(){
		pthread_mutex_destroy(&this->mutex_);
	}

	void lock(){
		pthread_mutex_lock(&this->mutex_);
	}

	void unlock(){
		pthread_mutex_unlock(&this->mutex_);
	}
};

}


#endif
#include "../publish_subscribe/publisher.cpp"

#include <string>
#include <iostream>
#include <pthread.h>
#include <assert.h>


using namespace simple;

void *run(void* arg){
	Publisher* p = static_cast<Publisher*>(arg);
	p->runThreading();
}

int main(){
	Publisher p;
	p.monitor("127.0.0.1", 8001);

	pthread_t id;
	int ret = pthread_create(&id, NULL, run, &p);
	assert(ret == 0);

	std::string a;
	while(true){
		std::getline (std::cin,a);
		p.publish(a.c_str(), strlen(a.c_str()));
	}

}
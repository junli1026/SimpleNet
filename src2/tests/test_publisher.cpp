#include "../publish_subscribe/publisher.h"

#include <string>
#include <iostream>
#include <pthread.h>
#include <assert.h>


using namespace simple;


int main(){
	Publisher p("127.0.0.1", 8001);
}

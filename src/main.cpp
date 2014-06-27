#include <vector>
#include <functional>
#include <iostream>
#include <string>

class Tester{
private:
	int data;
	
	Tester& operator=(const Tester& t){}
public:
	Tester(int d){this->data = d;}
	int getData(){return this->data;}
	Tester(const Tester& t){}
};


Tester f(){
	Tester t(3);
	return t;
}

int main(){
	auto t = f();
}
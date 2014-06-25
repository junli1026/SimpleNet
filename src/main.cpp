#include <vector>
#include <functional>
#include <iostream>
#include <string>

class Tester{
public:
	template<typename T>
	T Say(int a, std::function<T(int)> f){
		return f(a);
	}
};

int main(){
	/*
	auto f = [](int a ){
		std::cout << a << std::endl; 
		return std::string("hello world");
	};
	Tester t;
	std::string ret = t.Say<std::string>(12, f);
	std::cout << ret;
	*/

	std::function<int(int)> test;
	if(test == nullptr){
		std::cout << "null";
	}
}
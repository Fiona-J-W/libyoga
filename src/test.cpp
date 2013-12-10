#include <iostream>
#include <iomanip>
#include "format.hpp"


int main() {
	
	int tmp = 3;
	dlo2::writefln("char*: %s, char* as ptr: 0x%p, int*: 0x%p", "foo", "bar", &tmp);
	dlo2::writefln("uintptr_t: %_0-16s, casted ptr: %_0-16s", uintptr_t{345}, uintptr_t("foobar"));
	
	//std::cout << dlo2::format(
	//		"let's print something: %s, %s. and a std::string: %s, and ints: %s, uint: %s\n",
	//		"foobar",'c', std::string{"some string"}, 42, 1337u);
	/*
	if(argc != 2) return 1;
	
	if(argv[1][0] == 'p') {
		for(int i = 0; i < 100000000; ++i) {
			printf("i is %6d\n", i);
		}
	}
	else if(argv[1][0] == 's') {
		for(int i = 0; i < 100000000; ++i) {
			std::cout << "i is " <<std::setw(6) << i << std::endl;
		}
	}
	else if(argv[1][0] == 'w'){
		for(int i = 0; i < 100000000; ++i) {
			dlo2::writef("i is %-6s\n", i);
		}
	}
	else if(argv[1][0] == 'P') {
		auto str = "foobarbazblablub";
		for(int i = 0; i < 10000000; ++i) {
			printf("i is %6d, str='%s'\n", i, str);
		}
	}
	else if(argv[1][0] == 'S') {
		auto str = "foobarbazblablub";
		for(int i = 0; i < 10000000; ++i) {
			std::cout << "i is " <<std::setw(6) << i << ", str='"<< str <<"'\n" <<std::flush;
		}
	}
	else if(argv[1][0] == 'W'){
		auto str = "foobarbazblablub";
		for(int i = 0; i < 10000000; ++i) {
			dlo2::writef("i is %-6s, str='%s'\n", i, str);
		}
	}
	else return 2;
	*/
}

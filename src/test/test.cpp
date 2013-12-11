#include <iostream>
#include <iomanip>

#include "../include/yoga.hpp"


int main() {
	
	int tmp = 3;
	yoga::writefln("char*: %s, char* as ptr: 0x%p, int*: 0x%p", "foo", "bar", &tmp);
	yoga::writefln("uintptr_t: %_0-16s, casted ptr: %_0-16s", uintptr_t{345}, uintptr_t("foobar"));
	
	//yoga::ofile file{"test.txt"};
	//file.writef("some call to writef with parameter %s - ", 3);
	//file.writefln("some call to writefln with parameter %s", 42);
	
	DEBUG("just a test with an arg: %s", 1337);
	
	yoga::writefln("just some text without arguments");
	
	//std::cout << yoga::format(
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
			yoga::writef("i is %-6s\n", i);
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
			yoga::writef("i is %-6s, str='%s'\n", i, str);
		}
	}
	else return 2;
	*/
}

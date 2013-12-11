#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>

#include "../include/yoga.hpp"


int main() {
	
	int tmp = 3;
	yoga::writefln("char*: %s, char* as ptr: 0x%p, int*: 0x%p", "foo", "bar", &tmp);
	yoga::writefln("uintptr_t: %*0~16_16s, casted ptr: %*0~16_16s", uintptr_t{345}, uintptr_t("foobar"));
	
	//yoga::ofile file{"test.txt"};
	//file.writef("some call to writef with parameter %s - ", 3);
	//file.writefln("some call to writefln with parameter %s", 42);
	
	DEBUG("just a test with an arg: %s", 1337);
	DEBUGL(1, "");
	
	yoga::format("%s, %@0p, %s", "foo", 3);
	assert(yoga::format("a string with no args") == "a string with no args");
	assert(yoga::format("char: %s, unsigned char: %s, signed char: %s", 'c', uint8_t{1},
		int8_t{2}) == "char: c, unsigned char: 1, signed char: 2");
	
	yoga::writefln("just some text without arguments");
	
	yoga::writefln("print some vector: %s", std::vector<std::vector<int>>{{1,2},{3,4}});
	
	yoga::writefln("print some map: %s", std::map<std::string, int>{{"foo", 1},{"bar", 2},{"baz", 3}});
	
	yoga::writefln("print a tuple: %s", std::make_tuple(3, "foo", std::vector<int>{3,4,5}));
	
	yoga::writefln("true s: %s, false s: %s, true d: %d, false d: %d", true, false, true, false);
	
	yoga::writefln("float: %s, double: %s, long double: %s", 3.141f, 3.141592654, 2.718281828L);
	
	yoga::writefln("forward-range: %s", std::forward_list<int>{1,2,3,4,5});
}

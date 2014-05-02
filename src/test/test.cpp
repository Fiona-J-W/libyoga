#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>

#include "../include/yoga.hpp"


int fun2(int i1, int i2) {
	YOGA_TRACEF("Calling fun2 with %s and %s", i1, i2);
	YOGA_TRACEPOINT;
	return i1+i2;
}

void fun1() {
	YOGA_TRACEPOINT;
	fun2(3, 5);
	YOGA_TRACEPOINT;
}

int main() {
	yoga::settings::set_priority(yoga::priority::trace);
	//yoga::settings::set_print_location(true);
	//yoga::settings::set_print_time(true);
	YOGA_TRACEPOINT;
	YOGA_INFO("starting program");
	
	YOGA_WARN("nothing to do");
	YOGA_TRACEPOINT;
	YOGA_INFO("Writing some stuff directly:");
	yoga::writefln("testing format: 1:%s, 2:%s, 3:%%", 3, 4.5);
	yoga::writeln("testing normal writeln without further arguments");
	yoga::write("testing write with multiple arguments: ", 3, " foo ", 'b', "ar", '\n');
	YOGA_INFO("done");
	YOGA_TRACEPOINT;
	
	YOGA_INFO("testing the swrite-family");
	std::stringstream stream;
	yoga::swrite(stream, "foo ", "bar ", "baz");
	yoga::swriteln(stream);
	yoga::swritefln(stream, "%s, %s", "bla", "blub");
	if(stream.str() != "foo bar baz\nbla, blub\n") {
		YOGA_ERRORF("Stream contains unexpected data: “%s”", stream.str());
	}
	YOGA_INFO("done");
	YOGA_TRACEPOINT;
	
	std::vector<std::map<std::pair<std::string, int>, double>> insane_container{{{std::make_pair("foo", 3), 4.5}}};
	YOGA_INFO("some insane container: ", insane_container);
	YOGA_TRACEPOINT;
	
	std::forward_list<int> fwd_list{3,4,5,6};
	YOGA_INFO("some forward-list contains: ", fwd_list);
	YOGA_TRACEPOINT;
	
	YOGA_INFO("Calling some annotated function");
	fun1();
	YOGA_TRACEPOINT;
	
	YOGA_WARN("reaching end of programm");
	YOGA_FATAL(u8"Now it really ends…");
}

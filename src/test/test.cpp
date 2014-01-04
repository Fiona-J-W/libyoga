#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>

#include "../include/yoga.hpp"

int main() {
	yoga::settings::set_priority(yoga::priority::trace);
	YOGA_TRACEPOINT;
	YOGA_INFO("starting program");
	
	YOGA_WARN("nothing to do");
	YOGA_TRACEPOINT;
}

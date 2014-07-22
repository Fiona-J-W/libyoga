#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>
#include <string>

#include "../src/include/yoga.hpp"


int main() {
	using namespace std::literals;
	yoga::debug_printer printer;
	yoga::impl::format(printer, "foo{}bar{2}baz{1}", "bla", "blub", std::make_pair("meow"s, "bla"s));
}

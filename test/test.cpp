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
	yoga::impl::format(printer, "negative-integer: {}\n", -1);
	yoga::impl::format(printer, "0: {}\n", 0);
	yoga::impl::format(printer, "0U: {}\n", 0U);
	yoga::impl::format(printer, "foo{}bar{2}baz{1}\n", "bla", -123, std::make_pair("meow"s, 23));
}

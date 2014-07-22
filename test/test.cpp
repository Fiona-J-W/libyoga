#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>

#include "../src/include/yoga.hpp"


int main() {
	yoga::debug_printer printer;
	yoga::impl::format(printer, "foo{}bar{}baz{}", "bla", "blub", "meow");
	printer.flush();
}

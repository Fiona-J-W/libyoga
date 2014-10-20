#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>
#include <string>

#include "../src/include/yoga.hpp"
#include "../src/include/number_range.hpp"


int main() {
	using namespace std::literals;
	yoga::debug_printer printer;
	yoga::impl::format(printer, "negative-integer: {}\n", -1);
	yoga::impl::format(printer, "0: {}\n", 0);
	yoga::impl::format(printer, "0U: {}\n", 0U);
	yoga::impl::format(printer, "foo{}bar{2}baz{1}\n", "bla", -123, std::make_pair("meow"s, 23));
	yoga::impl::format(printer, "foo{{{}}}bar{{baz}}", 37);
	yoga::impl::format(printer, "{{}}");
	yoga::impl::format(printer, "{{");
	yoga::impl::format(printer, "\n");
	yoga::impl::format(printer, "{}", '\n');
	//printer.flush();
	
	{
		using iterator = yoga::fixed_number_iterator<int, 2>;
		iterator it1{2};
		iterator it2{4};
		assert(it1 != it2);
		it1++;
		assert(it1 == it2);
		assert(*it1 == 4);
		for(auto num: yoga::range(0, 7, 2)) {
			std::cout << num << '\n';
		}
		for(auto num: yoga::range<std::size_t>(3, 9, 4)) {
			std::cout << num << '\n';
		}
		for(auto num: yoga::make_fixed_number_range<std::size_t, 2>(3, 9)) {
			std::cout << num << '\n';
		}
	}
}

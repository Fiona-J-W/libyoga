#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <forward_list>
#include <string>

#include "../src/include/yoga.hpp"

class debug_printer: public yoga::printer_base<debug_printer> {
public:
	void flush() {
		std::cout << std::flush;
	}

	template <typename Iterator>
	void append(Iterator input_start, Iterator input_end) {
		std::copy(input_start, input_end, std::ostream_iterator<char>{std::cout});
	}

	using printer_base::append;
};

class string_printer: public yoga::printer_base<string_printer> {
public:
	using printer_base::flush;
	using printer_base::append;

	template <typename Iterator>
	void append(Iterator input_start, Iterator input_end) {
		std::copy(input_start, input_end, std::back_inserter(m_string));
	}

	const std::string& str() const& {return m_string;}
	std::string str() && {return std::move(m_string);}

private:
	std::string m_string;
};


int main() {
	using namespace std::literals;
	using namespace yoga::format_literals;

	debug_printer printer;
	printer.printf("negative-integer: {}\n", -1);
	printer.printf("0: {}\n", 0);
	printer.printf("0U: {}\n", 0U);
	printer.printf("foo{}bar{2}baz{1}\n", "bla", -123, std::make_pair("meow"s, 23));
	printer.printf("foo{{{}}}bar{{baz}}", 37);
	printer.printf("{{}}");
	printer.printf("{{");
	printer.printf("\n");
	printer.printf("{}", '\n');
	printer.printf("{}\n", yoga::fmt(38, 7_w, '0'_f, 13_base));
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

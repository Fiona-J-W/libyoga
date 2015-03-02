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


template<typename...Args>
void test_printf(const std::string& expected, const std::string& format, const Args&... args) {
	auto printer = string_printer{};
	printer.printf(format, args...);
	if (expected != printer.str()) {
		//assert(printer.str().back() != 0);
		throw std::logic_error{"printf didn't work for “" + format + "” (expected “" + expected + "”, actual “" + printer.str() + "”)" };
	}
}
	
using some_multirange = yoga::multi_range<std::vector<int>, std::vector<int>>&;
static_assert(std::is_same<decltype(std::begin(std::declval<some_multirange>())),
                           decltype(  std::end(std::declval<some_multirange>()))>::value, "");
static_assert(std::is_same<std::iterator_traits<decltype(std::begin(std::declval<some_multirange>()))>::iterator_category, std::forward_iterator_tag>::value, "");

int main() try {
	using namespace std::literals;
	using namespace yoga::format_literals;

	test_printf("foo", "foo");
	test_printf("foo", "{}", "foo");
	test_printf("3", "{}", 3);
	test_printf("-3", "{}", -3);
	test_printf("x", "{}", 'x');
	test_printf("4.5", "{}", yoga::fmt(4.5));
	//test_printf("4.500", "{}", yoga::fmt(4.5, 3_p)); // TODO: doesn't work yet
	test_printf("010", "{}", yoga::fmt(8, 3_w, '0'_f, 8_base));
	test_printf("{}", "{{}}");
	test_printf("{3}", "{{3}}");
	test_printf("{3}", "{{{}}}", 3);
	test_printf("(3, 7)", "{}", std::make_pair(3, 7));
	test_printf("(3, 7)", "{}", std::make_tuple(3, 7));
	test_printf("[1, 2]", "{}", std::vector<int>{1, 2});
	test_printf("[(1, a)]", "{}", std::map<int, char>{{1, 'a'}});
	test_printf("[(1, [foo])]", "{}", std::vector<std::pair<int, std::forward_list<std::string>>>{{1, {"foo"}}});
	test_printf("2 1", "{1} {0}", 1, 2);
	test_printf("foo 010 [1, 2]", "{} {2} {1}", "foo", std::vector<int>{1, 2}, yoga::fmt(8, 3_w, '0'_f, 8_base));
	//test_printf("", "{}", );
	
	{
		using iterator = yoga::fixed_number_iterator<int, 2>;
		iterator it1{2};
		iterator it2{4};
		assert(it1 != it2);
		it1++;
		assert(it1 == it2);
		assert(*it1 == 4);
	}
	{
		std::vector<int> vec;
		for(auto num: yoga::range(0, 7, 2)) {
			vec.push_back(num);
		}
		auto expected = std::vector<int>{0,2,4,6};
		assert(vec == expected);
	}
	{
		std::vector<std::size_t> vec;
		for(auto num: yoga::range<std::size_t>(3, 9, 4)) {
			vec.push_back(num);
		}
		auto expected = std::vector<std::size_t>{3,7};
		assert(vec == expected);
	}
	{
		std::vector<std::size_t> vec;
		for(auto num: yoga::make_fixed_number_range<std::size_t, 2>(3, 9)) {
			vec.push_back(num);
		}
		auto expected = std::vector<std::size_t>{3,5,7};
		assert(vec == expected);
	}
	{
		auto printer = debug_printer{};
		using intvec = std::vector<int>;
		auto results = std::vector<std::pair<int, int>>();
		for(auto x: yoga::make_multi_range(intvec{1,2,3}, yoga::range<int>(3, 6))) {
			int i1,i2;
			std::tie(i1, i2) = x;
			results.emplace_back(i1, i2);
		}
		auto expected = std::vector<std::pair<int,int>>{{1,3}, {1,4}, {1,5},{2,3}, {2,4}, {2,5},{3,3}, {3,4}, {3,5}};
		assert(results == expected);
	}
	{
		auto printer = debug_printer{};
		printer.println(yoga::make_multi_range(yoga::range<int>(3,9,2), yoga::range(7,3, -1)));
	}
	{
		auto printer = debug_printer{};
		printer.println(yoga::make_filter_range(yoga::range(-10, 10), [](int i){return i > 0 or (i % 3) == 0;}));
	}
} catch(std::exception& e) {
	std::cerr << "Error: " << e.what() << '\n';
	return 2;
}

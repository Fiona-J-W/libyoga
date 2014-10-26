
#ifndef YOGA_WRITE_HPP
#define YOGA_WRITE_HPP

#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <cassert>
#include <iterator>
#include <cstring>
#include <sstream>
#include <string>

#include "pointer.hpp"
#include "enforce.hpp"
#include "number_parsing.hpp"
#include "tmp.hpp"
#include "format.hpp"

namespace yoga {


template<typename Self>
class printer_base;

namespace impl {

template <typename Output>
class basic_printer;

template <typename Output, typename Value>
class printer; // inherits basic_printer

template <typename Output, typename Value>
std::unique_ptr<basic_printer<Output> > make_unique_printer(Output& output, const Value& value);

// TODO: rename
template <typename Output, typename String, typename... Args>
void format(Output& output, const String& str, const Args&... args);

// Categories of how a type might be printable
enum class printable_category {
	unprintable,
	// TODO: the native handle
	pair,
	tuple,
	integer,
	string_literal,
	char_range,
	streamable,
	iteratable
};

template<typename T> constexpr bool is_integer();
template<typename T> constexpr bool is_streamable();
template<typename T> constexpr bool is_pair();
template<typename T> constexpr bool is_tuple();
template<typename T> constexpr bool is_iteratable();
template<typename T> constexpr bool is_string_literal();
template<typename T> constexpr bool is_char_range();

template<typename T>
constexpr printable_category get_printable_category();

template<printable_category Tag> struct printable_category_tag{};
using pair_tag           = printable_category_tag< printable_category::pair          >;
using tuple_tag          = printable_category_tag< printable_category::tuple         >;
using integer_tag        = printable_category_tag< printable_category::integer       >;
using iteratable_tag     = printable_category_tag< printable_category::iteratable    >;
using string_literal_tag = printable_category_tag< printable_category::string_literal>;
using char_range_tag     = printable_category_tag< printable_category::char_range    >;
using streamable_tag     = printable_category_tag< printable_category::streamable    >;
using unprintable_tag    = printable_category_tag< printable_category::unprintable   >;


template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, pair_tag);

template <typename Output, typename Value>
void print(Output& output, Value value,        const impl::print_format&, integer_tag);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, tuple_tag);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, string_literal_tag);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, char_range_tag);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, iteratable_tag);

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, streamable_tag);




///////////////////////////////////////////////////////////////////////

// definitions:
template <typename Output>
class basic_printer {
public:
	virtual ~basic_printer() = default;
	virtual void print() const = 0;

protected:
	basic_printer(Output& output) : output{ output } {}

	reference<Output> output;
};

template <typename Output, typename Value>
class printer final : public basic_printer<Output> {
public:
	printer(Output& output, const Value& value)
	    : basic_printer<Output>{ output }, value{ value } {}
	void print() const override final {
		yoga::impl::print(*(this->output), *value, {});
	}

private:
	reference<const Value> value;
};

template <typename Output, typename Value>
class printer<Output, impl::format_pair<Value>> final : public basic_printer<Output> {
public:
	printer(Output& output, const impl::format_pair<Value>& value)
	    : basic_printer<Output>{ output }, value{ value } {}
	void print() const override final {
		yoga::impl::print(*(this->output), value->value, value->format);
	}

private:
	reference<const impl::format_pair<Value>> value;
};

template <typename Output, typename Value>
std::unique_ptr<basic_printer<Output> > make_unique_printer(Output& output, const Value& value) {
	return std::make_unique<printer<Output, Value> >(output, value);
}

template <typename Output, typename String, typename... Args>
void format(Output& output, const String& str, const Args&... args) {
	const auto printers = std::array<std::unique_ptr<basic_printer<Output> >, sizeof...(Args)>{
		{ make_unique_printer(output, args)... }};
	const auto splitters = std::array<char, 2>{{'{', '}'}};
	const auto end = std::end(str);
	const auto find_next = [&](auto it) {return std::find_first_of(it, end, splitters.begin(), splitters.end());};

	std::size_t index = 0;
	auto it_old = std::begin(str);
	auto it = find_next(it_old);
	for(; it_old != end; it = find_next(it_old)) {
		output.append(it_old, it);
		if (it == end) {
			break;
		}
		if (*it == '}') {
			++it;
			if (it == end or *it != '}') {
				throw std::invalid_argument{"invalid formatstring"};
			}
			output.append('}');
			++it;
			it_old = it;
			continue;
		}
		assert(*it == '{');
		++it;
		if (it == end) {
			throw std::invalid_argument{"invalid formatstring"};
		}
		if (*it == '{') {
			output.append('{');
		} else if (*it == '}') {
			printers.at(index)->print();
			++index;
		} else if (std::isdigit(*it)) {
			auto index = std::size_t{};
			std::tie(index, it) = ::yoga::str_to<std::size_t>(it, end);
			if (it == end or *it != '}') {
				throw std::invalid_argument{"invalid formatstring"};
			}
			printers.at(index)->print();
		} else {
			throw std::invalid_argument{"invalid formatstring"};
		}
		++it;
		it_old = it;
	}
}

// Now: let's implement how to print a single value of arbitrary type:
//////////////////////////////////////////////////////////////////////

template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format& f) {
	print(output, value, f, printable_category_tag<get_printable_category<Value>()>{});
}

// Integer
template <typename Output, typename Value>
void print(Output& output, Value value, const impl::print_format& f, integer_tag) {
	// prepare for all bases, including 2:
	std::array<char, sizeof(Value) * 8 + std::is_signed<Value>::value> arr;
	assert(arr.size() >= f.width()); // TODO: remove this restriction
	auto it = arr.begin();
	bool print_minus = false;
	if (value < 0) {
		print_minus = true;
		value = -value; // TODO: allow int-min
	}
	while (value != 0) {
		auto d = value%f.base();
		value /= f.base();
		if (d < 10) {
			*it = d + '0';
		} else {
			*it = (d-10) + 'a';
		}
		++it;
	}
	if (it == arr.begin()) {
		*it = '0';
		++it;
	}
	auto written_bytes = std::distance(arr.begin(), it);
	if (written_bytes < f.width()) {
		std::memset(arr.data() + written_bytes, f.fill(), f.width() - written_bytes);
		it += f.width() - written_bytes;
		if(print_minus) {
			*it = '-';
		}
	} else if (print_minus) {
		*it = '-';
		++it;
	}
	using r_iterator = decltype(arr.rbegin());
	output.append(r_iterator{it}, arr.rend());
}


// Pair
template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format& f, pair_tag) {
	
	output.append('(');
	print(output, value.first, f);
	output.append(", ");
	print(output, value.second, f);
	output.append(')');
}


// Tuple
template<typename Tuple, int I, int tupleSize> struct printtuple_helper {
	template<typename Output>
	static void print_(Output& output, const Tuple& arg, const print_format& f) {
		print(output, std::get<I-1>(arg), f);
		output.append(", ");
		printtuple_helper<Tuple, I+1, tupleSize>::print_(output, arg, f);
	}
};
template<typename Tuple, int I> struct printtuple_helper<Tuple, I, I>{
	template<typename Output>
	static void print_(Output& output, const Tuple& arg, const print_format& f) {
		print(output, std::get<I-1>(arg), f);
	}
};
template <typename Output, typename Value>
void print(Output& output, const Value& value, const print_format& f, tuple_tag) {
	output.append('(');
	impl::printtuple_helper<Value, 1, std::tuple_size<Value>::value>::print_(output, value, f);
	output.append(')');
}

// string-literals
template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, string_literal_tag) {
	output.append(std::begin(value), std::end(value) - 1);
}


// char-Range
template <typename Output, typename Value>
void print(Output& output, const Value& value, const print_format&, char_range_tag) {
	output.append(std::begin(value), std::end(value));
}


// Range
template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format& f, iteratable_tag) {
	auto it = std::begin(value);
	auto end = std::end(value);
	if (it == end) {
		output.append("[]");
		return;
	}
	output.append('[');
	print(output, *it, f);
	++it;
	while (it != end) {
		output.append(", ");
		print(output, *it, f);
		++it;
	}
	output.append(']');
}


// Streamable
template <typename Output, typename Value>
void print(Output& output, const Value& value, const impl::print_format&, streamable_tag) {
	std::ostringstream stream;
	stream << value;
	auto str = stream.str();
	output.append(str.begin(), str.end());
}



// Brace Yourself: Templatemetaprogramming is comming
/////////////////////////////////////////////////////

// These templates exist in order to detect whether a given type
// falls into a certain category of printable stuff

template<typename T>
constexpr printable_category get_printable_category() {
	return
		is_pair<T>()          ? printable_category::pair           :
		is_tuple<T>()         ? printable_category::tuple          :
		is_integer<T>()       ? printable_category::integer        :
		is_string_literal<T>()? printable_category::string_literal :
		is_char_range<T>()    ? printable_category::char_range     :
		is_streamable<T>()    ? printable_category::streamable     :
		is_iteratable<T>()    ? printable_category::iteratable     :
		/* else: */             printable_category::unprintable    ;
}


// integer (the real ones, not the stuff like bool and characters)
template<typename T> constexpr bool is_integer() {
	using Arg = std::decay_t<T>;
	return std::is_integral<Arg>:: value && !(
		std::is_same<Arg, bool>::value ||
		std::is_same<Arg, char>::value ||
		std::is_same<Arg, wchar_t>::value ||
		std::is_same<Arg, char16_t>::value ||
		std::is_same<Arg, char32_t>::value
	);
}

// iteratable
struct is_iteratable_helper {
	static std::false_type is_iteratable(...);
	
	template<typename T,
		class Iterator = decltype(std::begin(std::declval<T>())),
		YOGA_REQUIRE_N(EndIteratorValid, std::is_same<Iterator, decltype(std::end(std::declval<T>()))>::value),
		YOGA_REQUIRE_N(HasInputIterator, is_base_or_same<std::input_iterator_tag,
			typename std::iterator_traits<Iterator>::iterator_category>())
	> static std::true_type is_iteratable(const T&);
};
template<typename T> constexpr bool is_iteratable() {
	return decltype(is_iteratable_helper::is_iteratable(std::declval<T>()))::value;
}

// string-literal
std::false_type is_string_literal_helper(...);
template<std::size_t N>
std::true_type is_string_literal_helper(const char (&)[N]);
template<typename T> constexpr bool is_string_literal() {
	return decltype(is_string_literal_helper(std::declval<T>()))::value;
}

// char-range
struct is_char_range_helper {
	static std::false_type is_char_range(...);

	template<typename T,
		YOGA_REQUIRE_N(IsRange, is_iteratable<T>()),
		YOGA_REQUIRE_N(ValueTypeIsChar, std::is_same<std::decay_t<decltype(*std::begin(std::declval<T>()))>, char>::value)
	> static std::true_type is_char_range(const T&);

	// this is a hack, because for some reason the above is_iterateable doesn't work with build-in arrays
	static std::true_type is_char_range(const char[]);
};
template<typename T> constexpr bool is_char_range() {
	return decltype(is_char_range_helper::is_char_range(std::declval<T>()))::value;
}


// pair
template<typename T              > struct is_pair_helper                    : std::false_type {};
template<typename T1, typename T2> struct is_pair_helper<std::pair<T1, T2>> : std::true_type {};
template<typename T> constexpr bool is_pair() {
	return is_pair_helper<T>::value;
}

// tuple
template<typename   T> struct is_tuple_helper                   : std::false_type {};
template<typename...T> struct is_tuple_helper<std::tuple<T...>> : std::true_type {};
template<typename T> constexpr bool is_tuple() {
	return is_tuple_helper<T>::value;
}

// streamable
struct is_streamable_helper {
	static std::false_type is_streamable(...);
	
	template<typename T,
		YOGA_REQUIRE_N(streamable, is_base_or_same<std::ostream,
				std::decay_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>())
	>
	static std::true_type is_streamable(const T&);
	
};
template<typename T> constexpr bool is_streamable() {
	return decltype(is_streamable_helper::is_streamable(std::declval<T>()))::value;
}


} // namespace impl

template<typename Self>
class printer_base {
public:
	printer_base() noexcept = default;
	printer_base(const printer_base&) noexcept = default;
	printer_base(printer_base&&) noexcept = default;

	printer_base& operator=(const printer_base&) noexcept = default;
	printer_base& operator=(printer_base&&) noexcept = default;

	~printer_base() {
		get_self().flush();
	}

	template<typename...Args>
	void printf(Args&&... args) {
		impl::format(get_self(), std::forward<Args>(args)...);
	}
	template<typename...Args>
	void printfln(Args&&... args) {
		printf(std::forward<Args>(args)...);
		append('\n');
	}

	void flush() const noexcept {}
	
	template<std::size_t N>
	void append(const char(&str)[N]) {
		// don't append the 0-byte:
		get_self().append(str, str + N - 1);
	}

	void append(char c) {
		get_self().append(&c, &c + 1);
	}
private:
	Self& get_self() {
		return static_cast<Self&>(*this);
	}
};

} // namespace yoga

#endif

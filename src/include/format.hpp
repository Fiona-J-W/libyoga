#ifndef YOGA_FORMAT_HPP
#define YOGA_FORMAT_HPP

#include <algorithm>
#include <cassert>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <tuple>
#include <utility>

#include "util.hpp"

namespace yoga {

template<typename...T>
std::string to_string(const T&...args);

template<typename...T>
std::ostream& print_to_stream(std::ostream& stream, const T&...args);

template<typename...T>
std::string format(const std::string& format, const T&...args);

template<typename...T>
std::ostream& print_to_stream_formated(std::ostream& stream, const std::string& format, const T&...args);

// Implementation
/////////////////


namespace impl {


// Categories of how a type might be printable
enum class printable_category {
	unprintable,
	iteratable,
	pair,
	tuple,
	streamable,
};

template<typename T> constexpr bool is_streamable();
template<typename T> constexpr bool is_pair();
template<typename T> constexpr bool is_tuple();
template<typename T> constexpr bool is_iteratable();

template<typename T>
constexpr printable_category getprintable_category() {
	return
		is_streamable<T>() ? printable_category::streamable  :
		is_pair<T>()       ? printable_category::pair        :
		is_tuple<T>()      ? printable_category::tuple       :
		is_iteratable<T>() ? printable_category::iteratable  :
		/* else: */          printable_category::unprintable ;
}
template<printable_category Tag> struct printable_category_tag{};
using iteratable_tag   = printable_category_tag< printable_category::iteratable  >;
using pair_tag         = printable_category_tag< printable_category::pair        >;
using tuple_tag        = printable_category_tag< printable_category::tuple       >;
using streamable_tag   = printable_category_tag< printable_category::streamable  >;
using unprintable_tag  = printable_category_tag< printable_category::unprintable >;

template<typename T, typename...Args> void print_to_stream(std::ostream& stream, const T&, const Args&...);
inline void print_to_stream(std::ostream&) {}

template<typename T> void print_to_stream_tagged(std::ostream& stream, const T&, iteratable_tag);
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T&, pair_tag);
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T&, tuple_tag);
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T&, streamable_tag);

// Claim that this function exists somewhere else to keep the errors clean
// (calling this function is not a problem since the error is already caught earlier
// in print_to_stream, and calling it directly will result in a linker-error)
template<typename T> extern void print_to_stream_tagged(std::ostream& , const T&, unprintable_tag);

template<typename T, typename...Args>
void print_to_stream(std::ostream& stream, const T& arg, const Args&...args) {
	static_assert(getprintable_category<T>() != printable_category::unprintable,
			"print_to_stream must not be called with an unprintable argument");
	print_to_stream_tagged(stream, arg, printable_category_tag<getprintable_category<T>()>{});
	print_to_stream(stream, args...);
}

inline std::tuple<std::string::const_iterator, bool> printFormatPartToStream(std::ostream& stream,
		std::string::const_iterator begin, std::string::const_iterator end);

inline void print_to_stream_formated(std::ostream& stream, std::string::const_iterator format_begin,
		std::string::const_iterator format_end) {
	bool print_argument;
	using iterator = std::string::const_iterator;
	iterator it;
	std::tie(it, print_argument) = printFormatPartToStream(stream, format_begin, format_end);
	if (print_argument) {
		throw std::invalid_argument{"formatstring requests more arguments then provided"};
	}
}
template<typename T, typename...Args>
void print_to_stream_formated(std::ostream& stream, std::string::const_iterator format_begin,
		std::string::const_iterator format_end, const T& arg, const Args&...args) {
	bool print_argument;
	using iterator = std::string::const_iterator;
	iterator it;
	std::tie(it, print_argument) = printFormatPartToStream(stream, format_begin, format_end);
	if(print_argument) {
		print_to_stream(stream, arg);
		print_to_stream_formated(stream, it, format_end, args...);
	} else {
		assert(it == format_end);
		return;
	}
}

inline std::tuple<std::string::const_iterator, bool> printFormatPartToStream(std::ostream& stream,
		std::string::const_iterator begin, std::string::const_iterator end) {
	if (begin == end) {
		return std::make_tuple(end, false);
	}
	while (true) {
		auto nextPercent = std::find(begin, end, '%');
		stream.write(&*begin, nextPercent-begin);
		if(nextPercent == end) {
			return std::make_tuple(end, false);
		} else {
			begin = ++nextPercent;
			if(begin == end) {
				throw std::invalid_argument{"formatstrings must not end on unmatched '%'"};
			} else if (*begin == '%') {
				stream.put('%');
				++begin;
			} else if (*begin == 's') {
				++begin;
				return std::make_tuple(begin, true);
			} else {
				throw std::invalid_argument{"formatstring contains illegal format-specifier"};
			}
		}
	}
}

// Brace Yourself: Templatemetaprogramming is comming
/////////////////////////////////////////////////////

// iteratable
struct is_iteratable_helper {
	static std::false_type is_iteratable(...);
	
	template<typename T,
		class Iterator = decltype(std::begin(std::declval<T>())),
		YOGA_REQUIRE_N(EndIteratorValid, is_same<Iterator, decltype(std::end(std::declval<T>()))>()),
		YOGA_REQUIRE_N(HasInputIterator, is_base_or_same<std::input_iterator_tag,
			typename std::iterator_traits<Iterator>::iterator_category>())
	> static std::true_type is_iteratable(const T&);
};
template<typename T> constexpr bool is_iteratable() {
	return decltype(is_iteratable_helper::is_iteratable(std::declval<T>()))::value;
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
				decay<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>())
	>
	static std::true_type is_streamable(const T&);
	
};
template<typename T> constexpr bool is_streamable() {
	return decltype(is_streamable_helper::is_streamable(std::declval<T>()))::value;
}


// And now: implement the actual printing:
//////////////////////////////////////////

// streamable
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T& arg, streamable_tag) {
	stream << arg;
}

// pair
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T& arg, pair_tag) {
	stream << '(';
	print_to_stream(stream, arg.first);
	stream << ", ";
	print_to_stream(stream, arg.second);
	stream << ')';
}

// tuple
template<typename tuple, int I, int tupleSize> struct printtuple_helper {
	static void print(std::ostream& stream, const tuple& arg) {
		print_to_stream(stream, std::get<I-1>(arg));
		stream << ", ";
		printtuple_helper<tuple, I+1, tupleSize>::print(stream, arg);
	}
};
template<typename tuple, int I> struct printtuple_helper<tuple, I, I>{
	static void print(std::ostream& stream, const tuple& arg) {
		print_to_stream(stream, std::get<I-1>(arg));
	}
};
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T& arg, tuple_tag) {
	stream << '(';
	printtuple_helper<T, 1, std::tuple_size<T>::value>::print(stream, arg);
	stream << ')';
}

// iteratable
template<typename T> void print_to_stream_tagged(std::ostream& stream, const T& arg, iteratable_tag) {
	auto it = std::begin(arg);
	auto end = std::end(arg);
	bool firstpass = true;
	stream << '[';
	while(it != end) {
		if(firstpass) {
			firstpass = false;
		}
		else {
			stream << ", ";
		}
		print_to_stream(stream, *it);
		++it;
	}
	stream << ']';
}


} // namespace impl

// Finally: put together the public interface:
//////////////////////////////////////////////

template<typename...T>
std::string to_string(const T&...args) {
	std::stringstream stream;
	print_to_stream(stream, args...);
	return stream.str();
}

template<typename...T>
std::ostream& print_to_stream(std::ostream& stream, const T&...args) {
	impl::print_to_stream(stream, args...);
	return stream;
}

template<typename...T>
std::string format(const std::string& format, const T&...args) {
	std::stringstream stream;
	print_to_stream_formated(stream, format, args...);
	return stream.str();
}

template<typename...T>
std::ostream& print_to_stream_formated(std::ostream& stream, const std::string& format, const T&...args) {
	impl::print_to_stream_formated(stream, format.begin(), format.end(), args...);
	return stream;
}

} //namespace Aux


#endif

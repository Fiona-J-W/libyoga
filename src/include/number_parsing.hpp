#ifndef YOGA_NUMBER_PARSING_H
#define YOGA_NUMBER_PARSING_H

#include <algorithm>
#include <cassert>
#include <cctype>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "enforce.hpp"

namespace yoga {

template <typename Number, typename CharIterator, typename ValidationPolicy = checkers::enforcer>
std::tuple<Number, CharIterator> str_to(CharIterator it, CharIterator end);

namespace impl {

template <typename CharIterator>
std::tuple<CharIterator, char> drop_spaces(CharIterator it, CharIterator end);

template <typename Integer>
double power_of_10(Integer exp);

class integer_tag {};

template <typename Integer, typename CharIterator, typename ValidationPolicy>
std::tuple<Integer, CharIterator> str_to(CharIterator it, CharIterator end, integer_tag);

class real_tag {};

template <typename Real, typename CharIterator, typename ValidationPolicy>
std::tuple<Real, CharIterator> str_to(CharIterator it, CharIterator end, real_tag);

template <typename T>
using ArithmeticTag = typename std::conditional<
    std::is_integral<T>::value, integer_tag,
    typename std::conditional<std::is_floating_point<T>::value, real_tag, void>::type>::type;

} // namespace impl

/**
 * Parses a range of characters as number.
 *
 * @param Number must be either a floating-point-type or an integer-type
 * @param CharIterator must be a valid input-iterator over a type that is
 *                     implicitly convertable to char
 * @param ValidationPolicy must be a type that is compatible to the checkers from Enforce.h,
 *                         the default is Asserter, which will check conditions via assert()
 * @param it the start of the character-range
 * @param end the end of the character-range
 *
 * Requirements: The range [it, end) must contain a valid number.
 *
 * @return: A tuple of the parsed value and the iterator after parsing the number and dropping
 *           any surrounding whitespace.
 *
 */
template <typename Number, typename CharIterator, typename ValidationPolicy>
std::tuple<Number, CharIterator> str_to(CharIterator it, CharIterator end) {
	return impl::str_to<Number, CharIterator, ValidationPolicy>(it, end,
	                                                            impl::ArithmeticTag<Number>{});
}

namespace impl {

template <typename Integer, typename CharIterator, typename ValidationPolicy>
std::tuple<Integer, CharIterator> str_to(CharIterator it, const CharIterator end, integer_tag) {
	using impl::drop_spaces;
	ValidationPolicy::enforce(it != end);

	char c = *it;
	std::tie(it, c) = drop_spaces(it, end);

	bool is_negative = false;
	if (std::is_signed<Integer>::value) { // this should be optimized away entirely
		switch (c) {
		case '-':
			is_negative = true;
		// fallthrough
		case '+':
			++it;
			if (it == end) {
				throw std::invalid_argument{
					"string contains no digits after sign"
				};
			}
			c = *it;
			break;
		default:
			break;
		}
	}

	if (!isdigit(c)) {
		throw std::invalid_argument{ "string contains no digits" };
	}

	Integer val = 0;
	while (true) {
		ValidationPolicy::enforce(std::numeric_limits<Integer>::max() / 10 >= val);

		val *= 10;

		c -= '0';
		ValidationPolicy::enforce(std::numeric_limits<Integer>::max() - c >= val);

		val += c;

		++it;
		if (it == end) {
			break;
		}
		c = *it;
		if (!isdigit(c)) {
			break;
		}
	}

	if (is_negative) {
		val = -val;
	}

	std::tie(it, c) = drop_spaces(it, end);

	return std::make_tuple(val, it);
}

template <typename Real, typename CharIterator, typename ValidationPolicy>
std::tuple<Real, CharIterator> str_to(CharIterator it, const CharIterator end, real_tag) {

	static_assert(std::numeric_limits<Real>::max_digits10 <=
	                  std::numeric_limits<std::uintmax_t>::digits10,
	              "can't safe mantissa in biggest integer");

	using impl::drop_spaces;
	using impl::power_of_10;

	char c;
	bool is_negative = false;
	std::uintmax_t mantissa = 0;
	int exp = 0;

	auto make_return_value = [&]() {
		Real fp_mantissa = mantissa;
		Real value = fp_mantissa * power_of_10(exp);
		if (is_negative) {
			value = -value;
		}
		return std::make_tuple(value, it);
	};

	// drop whitespace:
	std::tie(it, c) = drop_spaces(it, end);

	ValidationPolicy::enforce(it != end);

	// set sign:
	switch (c) {
	case '-':
		is_negative = true;
	// fallthrough
	case '+':
		++it;
		if (it == end) {
			throw std::invalid_argument{ "string contains no digits" };
		}
		c = *it;
		break;
	default:
		break;
	}

	// number of decimal digits that can be stored in the mantissa and the used integer
	unsigned remainingDigits = std::numeric_limits<Real>::max_digits10;

	// read 'big' part of the mantissa:
	while (remainingDigits > 0) {
		if (!isdigit(c)) {
			break;
		}
		--remainingDigits;
		mantissa *= 10;
		mantissa += c - '0';
		++it;
		if (it == end) {
			return make_return_value();
		}
		c = *it;
	}
	if (remainingDigits == 0) {
		if (isdigit(c)) {
			// round correctly:
			if (c - '0' >= 5) {
				++mantissa;
			}
		}
		while (isdigit(c)) {
			++exp;
			++it;
			if (it == end) {
				break;
			}
			c = *it;
		}
	}
	// read 'small' part of the mantissa
	if (c == '.') {
		++it;
		if (it == end) {
			return make_return_value();
		}
		c = *it;
		while (remainingDigits > 0) {
			if (!isdigit(c)) {
				break;
			}
			--exp;
			--remainingDigits;
			mantissa *= 10;
			mantissa += c - '0';
			++it;
			if (it == end) {
				return make_return_value();
			}
			c = *it;
		}
		if (isdigit(c)) {
			// round correctly:
			if (c - '0' >= 5) {
				++mantissa;
			}
		}
		// skip the remaining digits:
		it = std::find_if_not(it, end, isdigit);
		if (it == end) {
			return make_return_value();
		}
		c = *it;
	}

	// calculate the final exponent:
	if (c == 'e' || c == 'E') {
		++it;
		int tmp;
		// we need to pass integer_tag explicitly here because we are in a
		// nested namespace. This shouldn't be required anywhere else
		std::tie(tmp, it) =
		    str_to<int, CharIterator, ValidationPolicy>(it, end, integer_tag{});
		exp += tmp;
	}

	// drop further whitespace:
	std::tie(it, c) = drop_spaces(it, end);

	return make_return_value();
}

template <typename CharIterator>
std::tuple<CharIterator, char> drop_spaces(CharIterator it, CharIterator end) {
	char c = 0;
	while (it != end && std::isspace((c = *it))) {
		++it;
	}
	return std::make_tuple(it, c);
}

template <typename Integer>
double power_of_10(Integer exp) {
	if (exp < 0) {
		return 1.0 / power_of_10(-exp);
	}
	switch (exp) {
	case 0:
		return 1;
	case 1:
		return 10;
	case 2:
		return 100;
	case 3:
		return 1000;
	case 4:
		return 10000;
	case 5:
		return 100000;
	case 6:
		return 1000000;
	case 7:
		return 10000000;
	case 8:
		return 100000000;
	case 9:
		return 1000000000;
	default:
		auto tmp = power_of_10(exp / 2);
		if (exp % 2 == 0) {
			return tmp * tmp;
		} else {
			return tmp * tmp * 10;
		}
	}
}

} // namespace impl

} // namespace yoga

#endif

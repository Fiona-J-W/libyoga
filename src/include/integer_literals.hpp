#ifndef YOGA_INTEGER_LITERALS_HPP
#define YOGA_INTEGER_LITERALS_HPP

/**
 * This header defines the following user-defined literals:
 *
 * _sz for std::size_t
 * _pd for std::ptrdiff_t
 * _u8, _u16, _u32, _u64 for the appropriate unsigned integers
 * _i8, _i16, _i32, _i64 for the appropriate signed integers
 *
 * It should be noted, that it is impossible to create the smallest
 * available value of signed integers. (This is an inherent limitation
 * caused by the fact that the minus is not part of the literal, not an
 * implementation-artifact of the library.)
 *
 * Trying to create an out-of-range value with these integers will result
 * in a guaranteed compilation-error, though not a beautifull one.
 *
 */

#include <cstdint>
#include <stdexcept>
#include <limits>

namespace yoga {
namespace impl {

template <unsigned Base>
constexpr inline unsigned char char_to_digit(char c) {
	return c >= '0' && c <= '0' + static_cast<char>(Base) ? c - '0'
	                                                      : throw std::invalid_argument{""};
}

template <>
constexpr inline unsigned char char_to_digit<16>(char c) {
	return
		c >= '0' && c <= '9' ? c - '0' :
		c >= 'a' && c <= 'f' ? c - 'a' + 10 :
		c >= 'A' && c <= 'F' ? c - 'A' + 10 :
		throw std::invalid_argument{""};
}

static_assert(char_to_digit<16>('a') == 10, "");

template <typename Integer>
constexpr Integer safe_positive_multiply(Integer l, Integer r) {
	return std::numeric_limits<Integer>::max() / l >= r ? l * r : throw std::overflow_error{""};
}

template <typename Integer>
constexpr Integer safe_positive_add(Integer l, Integer r) {
	return std::numeric_limits<Integer>::max() - l >= r ? l + r : throw std::overflow_error{""};
}

template <typename Integer, unsigned Base, char Head, char... Tail>
struct integer_parser_impl {
	const constexpr static Integer factor = safe_positive_multiply(
		integer_parser_impl<Integer, Base, Tail...>::factor, static_cast<Integer>(Base));
	const constexpr static Integer value =
		safe_positive_add(safe_positive_multiply(factor, Integer{char_to_digit<Base>(Head)}),
		                  integer_parser_impl<Integer, Base, Tail...>::value);
};

template <typename Integer, unsigned Base, char... Tail>
struct integer_parser_impl<Integer, Base, '\'', Tail...> {
	const constexpr static Integer factor = integer_parser_impl<Integer, Base, Tail...>::factor;
	const constexpr static Integer value = integer_parser_impl<Integer, Base, Tail...>::value;
};

template <typename Integer, unsigned Base, char C>
struct integer_parser_impl<Integer, Base, C> {
	const constexpr static Integer factor = 1;
	const constexpr static Integer value = char_to_digit<Base>(C);
};

template <typename Integer, char... Digits>
struct integer_parser
    : std::integral_constant<Integer, integer_parser_impl<Integer, 10, Digits...>::value> {};

template <typename Integer, char... Digits>
struct integer_parser<Integer, '0', 'b', Digits...>
    : std::integral_constant<Integer, integer_parser_impl<Integer, 2, Digits...>::value> {};

template <typename Integer, char... Digits>
struct integer_parser<Integer, '0', 'x', Digits...>
    : std::integral_constant<Integer, integer_parser_impl<Integer, 16, Digits...>::value> {};

template <typename Integer, char... Digits>
struct integer_parser<Integer, '0', Digits...>
    : std::integral_constant<Integer, integer_parser_impl<Integer, 8, Digits...>::value> {};

template <typename Integer>
struct integer_parser<Integer, '0'> : std::integral_constant<Integer, 0> {};

} // namespace impl

inline namespace literals {
inline namespace integer_literals {

template <char... Digits>
constexpr std::size_t operator"" _sz() {
	return impl::integer_parser<std::size_t, Digits...>::value;
}

template <char... Digits>
constexpr std::uint8_t operator"" _u8() {
	return impl::integer_parser<std::uint8_t, Digits...>::value;
}

template <char... Digits>
constexpr std::uint16_t operator"" _u16() {
	return impl::integer_parser<std::uint16_t, Digits...>::value;
}

template <char... Digits>
constexpr std::uint32_t operator"" _u32() {
	return impl::integer_parser<std::uint32_t, Digits...>::value;
}

template <char... Digits>
constexpr std::uint64_t operator"" _u64() {
	return impl::integer_parser<std::uint64_t, Digits...>::value;
}

template <char... Digits>
constexpr std::ptrdiff_t operator"" _pd() {
	return impl::integer_parser<std::ptrdiff_t, Digits...>::value;
}

template <char... Digits>
constexpr std::int8_t operator"" _i8() {
	return impl::integer_parser<std::int8_t, Digits...>::value;
}

template <char... Digits>
constexpr std::int16_t operator"" _i16() {
	return impl::integer_parser<std::int16_t, Digits...>::value;
}

template <char... Digits>
constexpr std::int32_t operator"" _i32() {
	return impl::integer_parser<std::int32_t, Digits...>::value;
}

template <char... Digits>
constexpr std::int64_t operator"" _i64() {
	return impl::integer_parser<std::int64_t, Digits...>::value;
}

} // namespace integer_literals
} // namespace literals
} // namespace yoga

#endif

#ifndef YOGA_UTIL_HPP
#define YOGA_UTIL_HPP

#include <type_traits>

#define YOGA_REQUIRE(...) class = typename ::std::enable_if<__VA_ARGS__>::type
#define YOGA_REQUIRE_N(what, ...) class what = typename ::std::enable_if<__VA_ARGS__>::type

namespace yoga {

template<typename T> using decay = typename std::decay<T>::type;

template<bool B> using bool_to_type = std::integral_constant<bool, B>;

template<typename T1, typename T2>
constexpr bool is_same() {
	return std::is_same<T1, T2>::value;
}

template<typename Base, typename Derived>
constexpr bool is_base_or_same() {
	return is_same<Base, Derived>() || std::is_base_of<Base, Derived>::value;
}

} // namespace yoga

#endif

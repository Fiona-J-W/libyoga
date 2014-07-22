#ifndef YOGA_TMP_HPP
#define YOGA_TMP_HPP

#include <type_traits>

#define YOGA_REQUIRE(...) typename = std::enable_if_t<__VA_ARGS__>
#define YOGA_REQUIRE_N(name, ...) typename name = std::enable_if_t<__VA_ARGS__>

namespace yoga {

template <typename Base, typename Derived>
constexpr bool is_base_or_same() {
	return std::is_same<Base, Derived>::value || std::is_base_of<Base, Derived>::value;
}
}

#endif

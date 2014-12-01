#ifndef YOGA_UTIL_HPP
#define YOGA_UTIL_HPP

#include <array>
#include <type_traits>

namespace yoga {

template<typename...Args>
auto make_array(Args&&... args) {
	return std::array<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>{{std::forward<Args>(args)...}};
}

template<typename T, typename...Args>
auto make_array_of(Args&&... args) {
	return std::array<T, sizeof...(Args)>{{std::forward<Args>(args)...}};
}

} // namespace yoga

#endif

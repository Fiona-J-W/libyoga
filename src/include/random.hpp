#ifndef YOGA_RANDOM_HPP
#define YOGA_RANDOM_HPP

#include <random>
#include <cstdint>
#include <limits>

namespace yoga{

inline std::mt19937_64& get_urng() {
	thread_local static std::mt19937_64 urng{std::random_device{}()};
	return urng;
}

template<typename Integer = std::size_t>
inline Integer rand_int() {
	std::uniform_int_distribution<Integer> dist{0, std::numeric_limits<Integer>::max()};
	return dist(get_urng());
}

template<typename Integer = std::size_t>
inline Integer rand_int(Integer max) {
	std::uniform_int_distribution<Integer> dist{0, max};
	return dist(get_urng());
}

template<typename Integer = std::size_t>
inline Integer rand_int(Integer min, Integer max) {
	std::uniform_int_distribution<Integer> dist{min, max};
	return dist(get_urng());
}

} // namespace yoga

#endif // YOGA_RANDOM_HPP

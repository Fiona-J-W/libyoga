#ifndef YOGA_ZIP_HPP
#define YOGA_ZIP_HPP

#include <tuple>
#include <iterator>
#include <utility>

namespace yoga {

template <typename... Iterators>
class zipped_iterator {
public:

private:
	std::tuple<Iterators...> m_iterators;
	bool m_reached_end;
};

template <typename... Ranges>
class zipped_range {
public:
	template <typename... ArgRanges>
	zipped_range(ArgRanges&&... ranges)
	        : m_ranges{std::forward<ArgRanges>(ranges)...} {}

private:
	std::tuple<Ranges...> m_ranges;
};

} // namespace yoga

#endif

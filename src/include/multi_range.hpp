
#ifndef MULTI_RANGE_HPP
#define MULTI_RANGE_HPP

#include <utility>
#include <iterator>
#include <tuple>
#include <cstdint>
#include <type_traits>

#include "pointer.hpp"

namespace yoga {

template<typename MultiRange> class multi_iterator;
template<typename... Ranges> class multi_range;

template<typename... Ranges> multi_range<Ranges...> make_multi_range(Ranges&&...);

namespace impl {
template<std::size_t I> struct index_t : std::integral_constant<std::size_t, I> {};
template<typename Container> using iterator_t = decltype(std::begin(std::declval<Container>()));
template<typename Iterator> using value_type_t = typename std::iterator_traits<Iterator>::value_type;
template<typename Container> using container_value_type_t = typename std::iterator_traits<decltype(std::begin(std::declval<Container>()))>::value_type;

template<template<typename> class, typename>
struct apply_to_tuple_members{};

template<template<typename> class Metafunction, typename...Ts>
struct apply_to_tuple_members<Metafunction, std::tuple<Ts...>>{
	using type = std::tuple<Metafunction<Ts>...>;
};
template<template<typename> class Metafunction, typename T>
using apply_to_tuple_members_t = typename apply_to_tuple_members<Metafunction, T>::type;
} // namespace impl

template<typename... Ranges>
class multi_range {
public:
	template<typename... ArgumentRanges>
	multi_range(ArgumentRanges&&... args) : m_ranges{std::forward<ArgumentRanges>(args)...} {}

	using iterator_tuple_t = std::tuple<impl::iterator_t<std::add_const_t<Ranges>>...>;
	using iterator = multi_iterator<multi_range<Ranges...>>;
	using value_type = std::tuple<std::decay_t<impl::container_value_type_t<std::add_const_t<Ranges>>>...>;

	auto begin() const {return begin_impl(std::make_index_sequence<num_ranges()>{});}
	auto end() const {return end_impl(std::make_index_sequence<num_ranges()>{});}

	template<std::size_t Index>
	auto& get() {return std::get<Index>(m_ranges);}
	template<std::size_t Index>
	const auto& get() const {return std::get<Index>(m_ranges);}

	constexpr static std::size_t num_ranges() {return sizeof...(Ranges);}
private:
	template<std::size_t... Indexes>
	auto begin_impl(std::integer_sequence<std::size_t, Indexes...>) const {
		return iterator{*this, std::begin(get<Indexes>())...};
	}
	template<std::size_t... Indexes>
	auto end_impl(std::integer_sequence<std::size_t, 0, Indexes...>) const {
		return iterator{*this, std::end(get<0>()), std::begin(get<Indexes>())...};
	}

	std::tuple<std::add_const_t<Ranges>...> m_ranges;
};

template<typename MultiRange>
class multi_iterator {
public:
	//using value_type = std::decay_t<decltype(std::declval<multi_iterator<MultiRange>>().dereference())>;
	using value_type = typename MultiRange::value_type;
	using reference = impl::apply_to_tuple_members_t<std::add_lvalue_reference_t, value_type>;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;
	using pointer = impl::apply_to_tuple_members_t<std::add_lvalue_reference_t, value_type>*;

	template<typename...Args>
	multi_iterator(const MultiRange& mr, Args&&... args): m_multi_range{mr}, m_iterators{std::forward<Args>(args)...} {}

	friend bool operator==(const multi_iterator& lhs, const multi_iterator& rhs) {return lhs.m_iterators == rhs.m_iterators;}
	friend bool operator!=(const multi_iterator& lhs, const multi_iterator& rhs) {return !(lhs == rhs);}

	multi_iterator& operator++() {increment(); return *this;}
	multi_iterator operator++(int) {auto tmp = *this; increment(); return tmp;}

	auto operator*() {return dereference();}
	auto operator->() {return dereference();}

private:
	yoga::reference<const MultiRange> m_multi_range;
	typename MultiRange::iterator_tuple_t m_iterators;

	constexpr static std::size_t num_ranges() {return MultiRange::num_ranges();}

	// we cannot specialize member-templates of unspecialized templates,
	// so we have to emulate that behavior by overloading functions:
	// the returned bool tells, whether the iterators have to be set back to the beginning
	template<typename Index = impl::index_t<num_ranges() - 1>>
	void increment(Index = {});
	void increment(impl::index_t<0>);

	template<std::size_t Index>
	decltype(auto) dereference_impl() const {return *std::get<Index>(m_iterators);}

	template<std::size_t... Indexes>
	decltype(auto) dereference_impl(std::integer_sequence<std::size_t, Indexes...>) const {
		return std::tie(dereference_impl<Indexes>()...);
	}
	
	decltype(auto) dereference() const { 
		return dereference_impl(std::make_index_sequence<num_ranges()>{});
	}
};

template<typename MultiRange>
template<typename Index>
void multi_iterator<MultiRange>::increment(Index) {
	auto& range = m_multi_range->template get<Index::value>();
	auto& it = std::get<Index::value>(m_iterators);
	++it;
	if (it == std::end(range)) {
		increment(impl::index_t<Index::value - 1>{});
		it = std::begin(range);
	}
}

template<typename MultiRange>
void multi_iterator<MultiRange>::increment(impl::index_t<0>) {
	auto& it = std::get<0>(m_iterators);
	++it;
}

template<typename... Ranges> multi_range<Ranges...> make_multi_range(Ranges&&... ranges) {
	return multi_range<Ranges...>{std::forward<Ranges>(ranges)...};
}

} // namespace yoga


#endif


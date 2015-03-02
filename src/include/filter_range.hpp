
#ifndef YOGA_FILTER_RANGE_HPP
#define YOGA_FILTER_RANGE_HPP

#include <algorithm>
#include <cstdint>
#include <utility>
#include <iterator>

namespace yoga {

namespace impl {

template<typename Iterator>
struct iterator_category_helper {
private:
	using base_type = typename std::iterator_traits<Iterator>::iterator_category;
public:
	using type = std::conditional_t<std::is_same<base_type, std::random_access_iterator_tag>::value,
		std::bidirectional_iterator_tag, base_type>;
};
template<typename Iterator>
using iterator_category_helper_t = typename iterator_category_helper<Iterator>::type;

template<typename T>
struct rvalue_as_value{using type = T;};

template<typename T>
struct rvalue_as_value<T&&>{using type = T;};

template<typename T>
using rvalue_as_value_t = typename rvalue_as_value<T>::type;
}

template<typename Range, typename Filter>
class filter_range {
public:
	template<typename RangeArg>
	filter_range(RangeArg&& range, Filter filter):
		m_range{std::forward<RangeArg>(range)}, m_filter{filter} {}

	using base_iterator_type = decltype(std::begin(std::declval<const Range>()));
	using value_type = typename std::iterator_traits<base_iterator_type>::value_type;
	using reference = typename std::iterator_traits<base_iterator_type>::reference;
	using pointer = typename std::iterator_traits<base_iterator_type>::pointer;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	class iterator {
	public:
		using traits = std::iterator_traits<base_iterator_type>;
		using value_type = typename std::iterator_traits<base_iterator_type>::value_type;
		using reference = typename traits::reference;
		using pointer = typename std::iterator_traits<base_iterator_type>::pointer;
		using iterator_category = impl::iterator_category_helper_t<base_iterator_type>;
		using difference_type = std::ptrdiff_t;
		iterator(const filter_range& range, base_iterator_type it):
			m_range{&range}, m_it{std::move(it)} {}

		iterator() = delete;
		reference operator*() const {return *m_it;}
		//pointer operator->() const {return &*m_it;}
		iterator& operator++() {
			m_it = std::find_if(std::next(m_it), std::end(m_range->m_range), m_range->m_filter);
			return *this;
		}
		iterator& operator++(int) {
			assert(m_it != std::end(m_range->m_range));
			auto tmp = *this;
			++(*this);
			return tmp;
		}
		iterator& operator--() {
			assert(m_it != std::begin(m_range->m_range));
			using rev_it = std::reverse_iterator<iterator>;
			m_it = std::find_if(rev_it{m_it}, rev_it{std::begin(m_range->m_range)}, m_range->m_filter);
			return *this;
		}
		iterator& operator--(int) {
			auto tmp = *this;
			--(*this);
			return tmp;
		}
		friend bool operator==(const iterator& lhs, const iterator& rhs) {
			assert(lhs.m_range == rhs.m_range);
			return lhs.m_it == rhs.m_it;
		}
		friend bool operator!=(const iterator& lhs, const iterator& rhs) {
			return !(lhs == rhs);
		}
	private:
		const filter_range* m_range;
		base_iterator_type m_it;
	};
	iterator begin() const {return iterator{*this, std::find_if(std::begin(m_range), std::end(m_range), m_filter)};}
	iterator end() const {return {*this, std::end(m_range)};}
	size_type size() const {return std::distance(std::begin(m_range), std::end(m_range));}
private:
	Range m_range;
	Filter m_filter;
};

template<typename Range, typename Filter>
auto make_filter_range(Range&& range, Filter filter) {
	return filter_range<impl::rvalue_as_value_t<Range>, Filter>{std::forward<Range>(range), filter};
}


} // namespace yoga


#endif

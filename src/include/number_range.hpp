#ifndef YOGA_NUMBER_RANGE_HPP
#define YOGA_NUMBER_RANGE_HPP

#include <iterator>
#include <cstdint>

#include "pointer.hpp"

namespace yoga {

template<typename Number, Number Step = 1>
class fixed_number_iterator {
public:
	using value_type = Number;
	using reference = const Number&;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::random_access_iterator_tag;
	using pointer = pointer<const Number>;

	fixed_number_iterator(): current_value{0} {}
	fixed_number_iterator(Number start_value): current_value{start_value} {}

	reference operator*() const {
		return current_value;
	}
	pointer operator->() const {
		return &current_value;
	}
	
	fixed_number_iterator operator++() {
		current_value += Step;
		return *this;
	}

	fixed_number_iterator operator++(int) {
		auto tmp = *this;
		current_value += Step;
		return tmp;
	}

	fixed_number_iterator operator--() {
		current_value -= Step;
		return *this;
	}

	fixed_number_iterator operator--(int) {
		auto tmp = *this;
		current_value -= Step;
		return tmp;
	}

	Number operator[](difference_type n) {
		return current_value + (Step * n);
	}

	fixed_number_iterator operator+=(difference_type n) {
		current_value += n * Step;
		return *this;
	}

	fixed_number_iterator& operator+(difference_type n) const {
		return fixed_number_iterator{current_value + Step * n};
	}

	fixed_number_iterator operator-=(difference_type n) {
		current_value -= n * Step;
		return *this;
	}

	fixed_number_iterator& operator-(difference_type n) const {
		return fixed_number_iterator{current_value - Step * n};
	}

	friend bool operator==(fixed_number_iterator lhs, fixed_number_iterator rhs) {
		return lhs.current_value == rhs.current_value;
	}
	friend bool operator!=(fixed_number_iterator lhs, fixed_number_iterator rhs) { return !(lhs == rhs); }

	friend bool operator<(fixed_number_iterator lhs, fixed_number_iterator rhs) {
		return lhs.current_value < rhs.current_value;
	}
	friend bool operator>(fixed_number_iterator lhs, fixed_number_iterator rhs) { return rhs < lhs; }
	friend bool operator<=(fixed_number_iterator lhs, fixed_number_iterator rhs) { return !(rhs < lhs); }
	friend bool operator>=(fixed_number_iterator lhs, fixed_number_iterator rhs) { return !(lhs > rhs); }
private:
	Number current_value;
};

template<typename Number, Number Step = 1>
class fixed_number_range {
public:
	fixed_number_range(Number low, Number high): low{low}, high{high} {
		auto offset_high = high % Step;
		auto offset_low = low % Step;
		if (offset_low != offset_high) {
			this->high += -offset_high + offset_low;
			if (offset_high > offset_low) {
				this->high += Step;
			}
		}
	}
	fixed_number_iterator<Number, Step> begin() const {
		return {low};
	}
	fixed_number_iterator<Number, Step> end() const {
		return {high};
	}
private:
	Number low;
	Number high;
};

template<typename Number, Number Step = 1>
fixed_number_range<Number, Step> make_number_range(Number low, Number high) {
	return {low, high};
}

template<typename Number>
class number_iterator {
public:
	using value_type = Number;
	using reference = const Number&;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::random_access_iterator_tag;
	using pointer = pointer<const Number>;

	number_iterator(): current_value{0} {}
	number_iterator(Number start_value): current_value{start_value} {}
	number_iterator(Number start_value, Number step): current_value{start_value}, step{step} {}

	reference operator*() const {
		return current_value;
	}
	pointer operator->() const {
		return &current_value;
	}
	
	number_iterator operator++() {
		current_value += step;
		return *this;
	}
 
	number_iterator operator++(int) {
		auto tmp = *this;
		current_value += step;
		return tmp;
	}

	number_iterator operator--() {
		current_value -= step;
		return *this;
	}

	number_iterator operator--(int) {
		auto tmp = *this;
		current_value -= step;
		return tmp;
	}

	Number operator[](difference_type n) {
		return current_value + (step * n);
	}

	number_iterator operator+=(difference_type n) {
		current_value += n * step;
		return *this;
	}

	number_iterator& operator+(difference_type n) const {
		return number_iterator{current_value + step * n};
	}

	number_iterator operator-=(difference_type n) {
		current_value -= n * step;
		return *this;
	}

	number_iterator& operator-(difference_type n) const {
		return number_iterator{current_value - step * n};
	}

	friend bool operator==(number_iterator lhs, number_iterator rhs) {
		return lhs.current_value == rhs.current_value;
	}
	friend bool operator!=(number_iterator lhs, number_iterator rhs) { return !(lhs == rhs); }

	friend bool operator<(number_iterator lhs, number_iterator rhs) {
		return lhs.current_value < rhs.current_value;
	}
	friend bool operator>(number_iterator lhs, number_iterator rhs) { return rhs < lhs; }
	friend bool operator<=(number_iterator lhs, number_iterator rhs) { return !(rhs < lhs); }
	friend bool operator>=(number_iterator lhs, number_iterator rhs) { return !(lhs > rhs); }
private:
	Number current_value;
	Number step = 1;
};



template<typename Number>
class number_range {
public:
	number_range(Number low, Number high, Number step): low{low}, high{high}, step{step} {
		auto offset_high = high % step;
		auto offset_low = low % step;
		if (offset_low != offset_high) {
			this->high += -offset_high + offset_low;
			if (offset_high > offset_low) {
				this->high += step;
			}
		}
	}
	number_iterator<Number> begin() const {
		return {low, step};
	}
	number_iterator<Number> end() const {
		return {high, step};
	}
private:
	Number low;
	Number high;
	Number step;
};

template<typename Number>
number_range<Number> make_number_range(Number low, Number high, Number step) {
	return {low, high, step};
}

} // namespace yoga

#endif

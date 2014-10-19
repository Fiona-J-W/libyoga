
#ifndef YOGA_FMT_HPP
#define YOGA_FMT_HPP

#include <string>
#include <cstdint>

namespace yoga {

struct fmt_width{unsigned value;};
struct fmt_precission{unsigned value;};
struct fmt_fill{char value;};
struct fmt_base{unsigned value;};

inline namespace literals {
inline namespace fmt_literals {

constexpr fmt_width      operator"" _w(unsigned long long arg) {return {static_cast<unsigned>(arg)};}
constexpr fmt_precission operator"" _p(unsigned long long arg) {return {static_cast<unsigned>(arg)};}
constexpr fmt_fill       operator"" _f(char c)                 {return {c};}
constexpr fmt_base    operator"" _base(unsigned long long arg) {return {static_cast<unsigned>(arg)};}

} // namespace fmt_literals
} // namespace literals

namespace impl {

class print_format {
public:
	template<typename...Args>
	print_format(const Args&... args) {ignore(set(args)...);}

	auto width() const {return m_width.value;}
	auto precission() const {return m_precission.value;}
	auto fill() const {return m_fill.value;}
	auto base() const {return m_base.value;}

private:
	fmt_width m_width = {0};
	fmt_precission m_precission = {0};
	fmt_fill m_fill = {' '};
	fmt_base m_base = {10};

	// utility to be able to use parameter-pack expansion
	// to set all values instead of ugly recursive hacks.
	// this is also the reason why all setters have to return a value
	template<typename...T> void ignore(T...){}

	auto set(fmt_width width) {return m_width = width;}
	auto set(fmt_precission precission) {return m_precission = precission;}
	auto set(fmt_fill fill) {return m_fill = fill;}
	auto set(fmt_base base) {return m_base = base;}
};

template<typename T>
struct format_pair {
	const T& value;
	print_format format;
};

} // namespace impl

template<typename T, typename...Formats>
impl::format_pair<T> fmt(const T& value, const Formats&... formats) {
	return {value, print_format(formats...)};
}

} // namespace yoga

#ifdef LOCAL_TESTS
void testfun() {
	using namespace yoga::fmt_literals;
	auto var = yoga::print_format{37_w, '0'_f, 3_p};
	auto arg = yoga::fmt(42, 37_w, 3_p);
}
#endif // LOCAL_TESTS

#endif // YOGA_FMT_HPP


#ifndef YOGA_WRITE_HPP
#define YOGA_WRITE_HPP

#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>

#include <iostream> // for debugging
#include <string>

#include "pointer.hpp"

namespace yoga {

struct format {
	bool explicit_index = false;
	unsigned index = 0;
};

class debug_printer {
public:
	using buffer_t = std::vector<char>;

	debug_printer() {
		buffer.resize(4 * 1024);
		buffer_begin = buffer.end();
	}

	debug_printer(debug_printer&& rhs) {
		buffer = std::move(rhs.buffer);
		buffer_begin = rhs.buffer_begin;
	}

	debug_printer& operator=(debug_printer&& rhs) {
		buffer = std::move(rhs.buffer);
		buffer_begin = rhs.buffer_begin;
		return *this;
	}

	auto get_writeable_range() { return std::make_pair(buffer_begin, buffer.end()); }

	auto flush() {
		std::cout << std::string{ buffer.begin(), buffer_begin } << std::flush;
		buffer_begin = buffer.begin();
		return std::make_pair(buffer_begin, buffer.end());
	}

	auto set_written_until(buffer_t::iterator new_end) {
		buffer_begin = new_end;
		if (new_end == buffer.end()) {
			flush();
		}
	}

	template <typename Iterator>
	void write_range(Iterator input_start, Iterator input_end) {
		auto it = input_start;
		auto input_size = std::distance(it, input_end);
		auto output = get_writeable_range();
		auto output_size = std::distance(output.first, output.second);
		while (input_size > output_size) {
			std::copy_n(it, output_size, output.first);
			input_size -= output_size;
			it += output_size;
			output = flush();
			output_size = std::distance(output.first, output.second);
		}
		std::copy_n(it, input_size, output.first);
		set_written_until(output.first + input_size);
	}

private:
	buffer_t buffer;
	buffer_t::iterator buffer_begin;
};

namespace impl {

template <typename Output, typename Value>
void print(Output& output, const Value& value, const format&);

template <typename Output>
class basic_printer;

template <typename Output, typename Value>
class printer; // inherits basic_printer

template <typename Output, typename Value>
std::unique_ptr<basic_printer<Output> > make_unique_printer(Output& output, const Value& value);

template <typename Output, typename String, typename... Args>
void format(Output& output, const String& str, const Args&... args);

template <typename Iterator>
struct format parse_format(Iterator begin, Iterator end) {
	// for now: just ignore it
	(void)begin;
	(void)end;
	return {};
}

// definitions:

template <typename Output>
class basic_printer {
public:
	virtual ~basic_printer() = default;
	virtual void print(const struct format&) const = 0;

protected:
	basic_printer(Output& output) : output{ output } {}

	reference<Output> output;
};

template <typename Output, typename Value>
class printer final : public basic_printer<Output> {
public:
	printer(Output& output, const Value& value)
	    : basic_printer<Output>{ output }, value{ value } {}
	void print(const struct format& f) const override final {
		yoga::impl::print(*(this->output), *value, f);
	}

private:
	reference<const Value> value;
};

template <typename Output, typename Value>
std::unique_ptr<basic_printer<Output> > make_unique_printer(Output& output, const Value& value) {
	return std::make_unique<printer<Output, Value> >(output, value);
}

template <typename Output, typename String, typename... Args>
void format(Output& output, const String& str, const Args&... args) {
	std::array<std::unique_ptr<basic_printer<Output> >, sizeof...(Args)> printers{
		{ make_unique_printer(output, args)... }
	};

	std::size_t index = 0;
	using std::begin;
	using std::end;
	output.write_range(begin(str), end(str));
	for (auto&& printer : printers) {
		printer->print({});
	}
}

template <typename Output, typename Value>
void print(Output& output, const Value& value, const struct format&) {
	using std::begin;
	using std::end;
	output.write_range(begin(value), end(value));
}

} // namespace impl

} // namespace yoga

#endif

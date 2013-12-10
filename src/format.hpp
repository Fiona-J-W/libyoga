#ifndef DLO2_FORMAT_HPP
#define DLO2_FORMAT_HPP


#include <algorithm>
#include <array>
#include <climits>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#ifdef DLO2_USE_POSIX
#include <unistd.h>
#endif

namespace dlo2 {


template<typename...T>
std::string format(const std::string& format, const T&...args);


template<typename...T>
void writef(const std::string& format, const T&...args);


template<typename...T>
void writefln(std::string format, const T&...args);




////////////////////////////////////////////////////////////////////////
//                   Implementation starts here                       //
////////////////////////////////////////////////////////////////////////


namespace impl {

template<typename InputIterator>
size_t c_it_pair_to_uint(InputIterator& it, InputIterator end) {
	size_t returnval = 0;
	while(it != end && isdigit(*it)) {
		returnval *= 10;
		returnval += *it - '0';
		++it;
	}
	return returnval;
}

struct format_data {
	format_data(std::string::const_iterator str_begin, std::string::const_iterator str_end,
			size_t& default_index) :index{default_index} {
		specifier = *--str_end;
		bool increment_default_index = true;
		while(str_begin != str_end) {
			switch(*str_begin) {
				case '_':
					++str_begin != str_end ? fill = *str_begin :
						throw std::invalid_argument{"invalid formatstring"};
					++str_begin;
					break;
				case '-':
					width1 = c_it_pair_to_uint(++str_begin, str_end);
					break;
				case '.':
					width2 = c_it_pair_to_uint(++str_begin, str_end);
					break;
				
				default:
					throw std::invalid_argument{"invalid formatstring"};
			}
		}
		if(increment_default_index) {
			++ default_index;
		}
	}
	format_data(char spec, char fill, size_t index, size_t w1, size_t w2, size_t b):
		specifier{spec}, fill{fill}, index{index}, width1{w1}, width2{w2}, base{b} {}
	char specifier;
	char fill = ' ';
	size_t index;
	size_t width1 = 0;
	size_t width2 = 0;
	size_t base = 10;
};


class printer_interface {
public:
	virtual void print(std::string& outstring, const format_data& formatting) const = 0;
};

template<size_t Size>
using printer_list = std::array<printer_interface const*, Size>;

template<typename T>
class printer : public printer_interface {
	const T* value;
public:
	printer(const T& value) : value{&value} {}
	void set_value(const T& new_value) {value = &new_value;}
	void print(std::string& output, const format_data& format_data) const override {
		print_to_string(output, *value, format_data);
	}
};

void print_to_string(std::string& output, const std::string& value, const format_data& data) {
	if(data.specifier == 's') {
		output.append(value);
	}
	else throw std::invalid_argument{"invlid format-specifier for std::string"};
}
void print_to_string(std::string& output, char value, const format_data& data) {
	if(data.specifier == 's') {
		output.push_back(value);
	}
	else throw std::invalid_argument{"invlid format-specifier for std::string"};
}

template<size_t N>
void print_to_string(std::string& output, char value[N], const format_data& data) {
	if(data.specifier == 's') {
		output.append(value, value+N);
	}
	else throw std::invalid_argument{"invlid format-specifier for std::string"};
}

template<typename T, class = typename std::enable_if<std::is_integral<T>{}>::type,
		class = typename std::enable_if<std::numeric_limits<T>::is_signed>::type>
void print_to_string(std::string& output, T value, const format_data& data) {
	std::vector<char> buffer(std::max(sizeof(T)*CHAR_BIT + 2, data.width1 + 1), data.fill);
	auto it = buffer.end();
	bool is_negative = value < 0;
	if(is_negative) value = -value;
	auto base = data.base;
	if(value == 0) {
		*(--it) = '0';
	}
	else {
		auto int_to_hex = [](unsigned x){
			return x < 10 ? x + '0' : x -10 + 'a';
		};
		unsigned tmp;
		while(value) {
			tmp = value % base;
			value /= base;
			*(--it) = int_to_hex(tmp);
		}
		if(is_negative) {
			*(--it) = '-';
		}
	}
	if(static_cast<size_t>(buffer.end() - it) < data.width1) {
		it = buffer.end() - data.width1;
	}
	output.append(it, buffer.end());
}


template<typename T, class = typename std::enable_if<std::is_integral<T>{}>::type,
		class = typename std::enable_if<!std::numeric_limits<T>::is_signed>::type,
		class = void>
void print_to_string(std::string& output, T value, const format_data& data) {
	std::vector<char> buffer(std::max(sizeof(T)*CHAR_BIT + 2, data.width1 + 1), data.fill);
	auto it = buffer.end();
	auto base = data.base;
	if(value == 0) {
		*(--it) = '0';
	}
	else {
		auto int_to_hex = [](unsigned x){
			return x < 10 ? x + '0' : x - 10 + 'a';
		};
		unsigned tmp;
		while(value) {
			tmp = value % base;
			value /= base;
			*(--it) = int_to_hex(tmp);
		}
	}
	if(static_cast<size_t>(buffer.end() - it) < data.width1) {
		it = buffer.end() - data.width1;
	}
	output.append(it, buffer.end());
}

template<typename T>
void print_to_string(std::string& output, T const* value, const format_data& data) {
	static const format_data format{'s', '0', 0, 2*sizeof(size_t), 0, 16};
	if(data.specifier == 's' || data.specifier == 'p') {
		print_to_string(output, uintptr_t(value), format);
	}
	else {
		throw std::invalid_argument{""};
	}
}
template<>
void print_to_string(std::string& output, char const* value, const format_data& data) {
	static const format_data format{'s', '0', 0, 2*sizeof(size_t), 0, 16};
	if(data.specifier == 's') {
		output.append(value);
	}
	else if(data.specifier == 'p') {
		print_to_string(output, uintptr_t(value), format);
	}
	else {
		throw std::invalid_argument{""};
	}
}


template<size_t Index = 0, size_t ArraySize, typename Arg, typename...Args>
void prepare_printers(printer_list<ArraySize>& array,
		const Arg& arg, const Args&... args) {
	thread_local static printer<Arg> printer{arg};
	array.at(Index) = &printer;
	prepare_printers<Index + 1>(array, args...);
}
template<size_t Index = 0, size_t ArraySize>
inline void prepare_printers(printer_list<ArraySize>&){}

template<size_t ArraySize>
void format_impl(const std::string& formatstring, std::string& output,
		const printer_list<ArraySize>& printers) {
	auto formatslice_begin = formatstring.begin();
	auto formatslice_end = formatstring.begin();
	auto format_end = formatstring.end();
	size_t print_index = 0;
	while(formatslice_begin != format_end) {
		formatslice_end = std::find(formatslice_begin, format_end, '%');
		output.append(formatslice_begin, formatslice_end);
		if(formatslice_end == format_end) break;
		formatslice_begin = ++formatslice_end;
		if(formatslice_begin == format_end) {
			throw std::invalid_argument{"invalid formatstring"};
		}
		if (*formatslice_begin == '%') {
			output.push_back('%');
			formatslice_begin = ++formatslice_end;
		}
		else {
			formatslice_end = std::find_if(formatslice_begin, format_end, isalpha);
			if(formatslice_end == format_end) {
				throw std::invalid_argument{"invalid formatstring"};
			}
			++formatslice_end;
			format_data data{formatslice_begin, formatslice_end, print_index};
			if(data.index >= ArraySize) {
				throw std::invalid_argument{"formatstring requests nonexisting argument"};
			}
			printers[data.index]->print(output, data);
			formatslice_begin = formatslice_end;
		}
	}
}


template<typename...Args>
void format(const std::string& formatstring, std::string& output, const Args&...args) {
	thread_local static std::array<impl::printer_interface const*, sizeof...(Args)> printers;
	impl::prepare_printers(printers, args...);
	impl::format_impl(formatstring, output, printers);
}

} // namespace impl

template<typename...T>
std::string format(const std::string& format, const T&...args) {
	std::string returnstring;
	impl::format(format, returnstring, args...);
	return returnstring;
}


template<typename...T>
void writef(const std::string& formatstring, const T&...args) {
	thread_local static std::string buffer;
	buffer.clear();
	impl::format(formatstring, buffer, args...);
#ifdef DLO2_USE_POSIX
	write(1, static_cast<const void*>(buffer.c_str()), buffer.size());
#else
	// Don't use this if you don't have to:
	std::cout << buffer << std::flush;
#endif
}

template<typename...T>
void writefln(std::string format, const T&...args) {
	writef(format+'\n', args...);
}


} // namespace dlo2


//#include "format.tcc"
#endif


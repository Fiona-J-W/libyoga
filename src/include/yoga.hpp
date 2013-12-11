#ifndef YOGA_INCLUDE_GUARD_YOGA_HPP
#define YOGA_INCLUDE_GUARD_YOGA_HPP

/*
 *  libyoga - A modern output-library for C++
 *  Copyright (C) 2013 Florian Weber <florian.weber at sfz-bw.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <string>
#include <type_traits>

#ifdef YOGA_USE_POSIX
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#else
#	include <iostream>
#endif

namespace yoga {

/**
 * Create a string from a function-template, that behaves like a sane and typesafe sprintf.
 *
 * To get format-support for your class simply create a function in it's namespace:
 * void print_to_string(std::string& output, YOURCLASS arg, const yoga::impl::format_data& formatting)
**/
template<typename...T>
std::string format(const std::string& format, const T&...args);


template<typename...T>
void writef(const std::string& format, const T&...args);


template<typename...T>
void writefln(std::string format, const T&...args);


template<typename Derived>
class output_base {
public:
	template<typename...T>
	void writefln(const std::string& formatstring, const T&...args) {
		static_cast<Derived*>(this)->write(format(formatstring+'\n', args...));
	}
	
	template<typename...T>
	void writef(const std::string& formatstring, const T&...args) {
		static_cast<Derived*>(this)->write(format(formatstring, args...));
	}
};

class string_output : public output_base<string_output> {
	std::string string;
public:
	string_output(std::string other) : string{std::move(other)} {}
	
	void write(const std::string data) {
		string.append(data);
	}
	
	std::string str() const & { return string; }
	std::string str() && { return std::move(string); }
};

#ifdef YOGA_USE_POSIX
class ofile : public output_base<ofile> {
	int fd = -1;
public:
	
	static const int DEFAULT_FLAGS =  O_WRONLY | O_CREAT | O_TRUNC;
	static const mode_t DEFAULT_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	
	ofile() = default;
	
	ofile(const ofile&) = delete;
	ofile& operator=(const ofile&) = delete;
	
	ofile(ofile&& other) {
		std::swap(fd, other.fd);
	}
	ofile& operator=(ofile&& other) {
		std::swap(fd, other.fd);
		return *this;
	}
	
	explicit ofile(const char * const filename, int flags = DEFAULT_FLAGS,
			mode_t mode = DEFAULT_MODE) {
		if(!open(filename, flags, mode)) {
			throw std::system_error{std::error_code{}, "could not open file"};
		}
	}
	explicit ofile(const std::string& str, int flags = DEFAULT_FLAGS, mode_t mode = DEFAULT_MODE)
		: ofile{str.c_str(), flags, mode} {}
	
	~ofile() {
		close();
	}
	
	bool open(const char * const filename, int flags = DEFAULT_FLAGS, mode_t mode = DEFAULT_MODE) {
		close();
		if(filename) {
			fd = ::open(filename, flags, mode);
			return fd != -1;
		}
		else return true;
	}
	bool open(const std::string& filename, int flags = DEFAULT_FLAGS, mode_t mode = DEFAULT_MODE) {
		return open(filename.c_str(), flags, mode);
	}
	
	void write(const std::string& str) {
		if(fd == -1) {
			throw std::system_error{std::error_code{}, "ofile is closed"};
		}
		::write(fd, static_cast<const void*>(str.c_str()), str.size());
	};
	
	bool is_open() { return fd != -1; }
	
	void close() {
		if(fd != -1) {
			::close(fd);
			fd = -1;
		}
	}
	
	int sync() {
		if (fd != -1) {
			return fsync(fd);
		}
		else return -1;
	}
};

#endif

#ifdef YOGA_USE_DEBUGGING_UTILITIES

inline void set_debug_level(int level);

#ifdef YOGA_DEBUG

#ifdef YOGA_USE_PRETTY_FUNC
#define YOGA_FUNC  __PRETTY_FUNCTION__
#else
#define YOGA_FUNC __func__
#endif
#define DEBUG(...) ::yoga::impl::debug(__FILE__, __LINE__, YOGA_FUNC, 0, __VA_ARGS__)
#define DEBUGL(...) ::yoga::impl::debug(__FILE__, __LINE__, YOGA_FUNC, __VA_ARGS__)

#else

#define DEBUG(...)   do{}while(false)
#define DEBUGL(...)   do{}while(false)

#endif
#endif

////////////////////////////////////////////////////////////////////////
//                   Implementation starts here                       //
////////////////////////////////////////////////////////////////////////


namespace impl {

inline void write_to_stdout(const std::string& str);

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
	const T* value = nullptr;
public:
	printer() = default;
	printer(const T& value) : value{&value} {}
	printer& operator=(const T& value) {this->value = &value; return *this;}
	void set_value(const T& new_value) {value = &new_value;}
	void print(std::string& output, const format_data& format_data) const final override {
		print_to_string(output, *value, format_data);
	}
};

inline void print_to_string(std::string& output, const std::string& value, const format_data& data) {
	if(data.specifier == 's') {
		output.append(value);
	}
	else throw std::invalid_argument{"invlid format-specifier for std::string"};
}
inline void print_to_string(std::string& output, char value, const format_data& data) {
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
inline void print_to_string(std::string& output, char const* value, const format_data& data) {
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
	thread_local static printer<Arg> printer{};
	printer = arg;
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
				throw std::invalid_argument{
					"formatstring requests nonexisting argument"};
			}
			printers[data.index]->print(output, data);
			formatslice_begin = formatslice_end;
		}
	}
}

template<>
inline void format_impl(const std::string& formatstring, std::string& output,
		const printer_list<0>&) {
	auto it1 = formatstring.begin();
	auto it2 = it1;
	auto end = formatstring.end();
	while(it1 != end) {
		it2 = std::find(it1, end, '%');
		if(it1 != it2) {
			output.append(it1, it2);
		}
		if(it2 == end) {
			break;
		}
		++it2;
		if(it2 == end || *it2 != '%') {
			throw std::invalid_argument{"invalid formatstring"};
		}
		else {
			output.push_back('%');
			it1 = it2;
		}
	}
}


template<typename...Args>
void format(const std::string& formatstring, std::string& output, const Args&...args) {
	thread_local static std::array<impl::printer_interface const*, sizeof...(Args)> printers;
	impl::prepare_printers(printers, args...);
	impl::format_impl(formatstring, output, printers);
}

inline std::atomic_int& get_debug_level() {
	static std::atomic_int level{10};
	return level;
}

template<typename...T>
void debug(const char * const file, int line, const char * const func, int level,
		const std::string& formatstring, const T&...args) {
	if(level <= get_debug_level().load()) {
		std::string outstr;
#ifdef YOGA_USE_POSIX
		thread_local static const int pid = getpid();
		format("DEBUG(%s) ['%s', #%s, '%s', pid %s]: ", outstr, level, file, line,
				func, pid);
#else
		format("DEBUG(%s) ['%s', #%s, '%s']: ", outstr, level, file, line, func);
#endif
		format(formatstring, outstr, args...);
		outstr += '\n';
		write_to_stdout(outstr);
	}
}

inline void write_to_stdout(const std::string& str) {
	static std::mutex m;
	std::lock_guard<std::mutex> g{m};
#ifdef YOGA_USE_POSIX
	write(1, static_cast<const void*>(str.c_str()), str.size());
#else
	// Don't use this if you don't have too:
	std::cout << str << std::flush;
#endif
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
	impl::write_to_stdout(buffer);
}

template<typename...T>
void writefln(std::string format, const T&...args) {
	writef(format+'\n', args...);
}

inline void set_debug_level(int level) {
	impl::get_debug_level().store(level);
}

} // namespace yoga

#endif


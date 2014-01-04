#ifndef YOGA_PRINT_HPP
#define YOGA_PRINT_HPP

#include <iosfwd>
#include <iostream>
#include <mutex>

#include "format.hpp"

namespace yoga {

struct location {
	const char* file;
	const char* function;
	const int line;
};

enum class priority {
	trace,
	debug,
	info,
	warn,
	error,
	fatal
};

namespace settings {

priority get_priority();
void set_priority(priority p);

void set_print_time(bool b);
bool get_print_time();

void set_print_location(bool b);
bool get_print_location();

void set_logfile(const std::string& filename);
}

template<typename...Args>
void write(const Args&...args);

template<typename...Args>
void writeln(const Args&...args);

template<typename...Args>
void writef(const std::string& format, const Args&...args);

template<typename...Args>
void writefln(const std::string& format, const Args&...args);


template<typename...Args>
void swrite(std::ostream& stream, const Args&...args);

template<typename...Args>
void swriteln(std::ostream& stream, const Args&...args);

template<typename...Args>
void swritef(std::ostream& stream, const std::string& format, const Args&...args);

template<typename...Args>
void swritefln(std::ostream& stream, const std::string& format, const Args&...args);

// IMPLEMENATION
/////////////////////////////////////////////////////////////

namespace impl {
void log(const location& loc, priority p, const std::string msg);

template<typename...T>
void log(const location& loc, priority p, const T&...args) {
	if(p >= settings::get_priority()) {
		impl::log(loc, p, to_string(args...));
	}
}

template<typename...T>
void logf(const location& loc, priority p, const std::string& format, const T&...args) {
	if(p >= settings::get_priority()) {
		impl::log(loc, p, format(args...));
	}
}

} //namespace impl

template<typename...Args>
void write(const Args&...args) {
	print_to_stream(std::cout, args...);
}

template<typename...Args>
void writeln(const Args&...args) {
	print_to_stream(std::cout, args..., '\n');
}

template<typename...Args>
void writef(const std::string& format, const Args&...args) {
	print_to_stream_formated(std::cout, format, args...);
}

template<typename...Args>
void writefln(const std::string& format, const Args&...args) {
	print_to_stream_formated(std::cout, format + '\n', args...);
}

template<typename...Args>
void swrite(std::ostream& stream, const Args&...args) {
	print_to_stream(stream, args...);
}

template<typename...Args>
void swriteln(std::ostream& stream, const Args&...args) {
	print_to_stream(stream, args..., '\n');
}

template<typename...Args>
void swritef(std::ostream& stream, const std::string& format, const Args&...args) {
	print_to_stream_formated(stream, format, args...);
}

template<typename...Args>
void swritefln(std::ostream& stream, const std::string& format, const Args&...args) {
	print_to_stream_formated(stream, format + '\n', args...);
}

} // namespace yoga

#endif

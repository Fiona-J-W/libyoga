#ifndef AUXILIARY_LOG_H_
#define AUXILIARY_HOG_H_

#include <iostream>
#include <mutex>

#include "format.hpp"

#ifdef NOLOGGING

#define FATAL(...) do{}while(false)
#define ERROR(...) do{}while(false)
#define WARN(...)  do{}while(false)
#define INFO(...)  do{}while(false)
#define DEBUG(...) do{}while(false)
#define TRACE(...) do{}while(false)

#define FATALF(...) do{}while(false)
#define ERRORF(...) do{}while(false)
#define WARNF(...)  do{}while(false)
#define INFOF(...)  do{}while(false)
#define DEBUGF(...) do{}while(false)
#define TRACEF(...) do{}while(false)

#define TRACEPOINT do{}while(false)

#else // NOLOGGING

#define FATAL(...) ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define ERROR(...) ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define WARN(...)  ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define INFO(...)  ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define DEBUG(...) ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define TRACE(...) ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define FATALF(...) ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define ERRORF(...) ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define WARNF(...)  ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define INFOF(...)  ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define DEBUGF(...) ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define TRACEF(...) ::yoga::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define TRACEPOINT ::yoga::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, "tracepoint")

#endif // NOLOGGING

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

namespace impl {
void log(const location& loc, priority p, const std::string msg);
} //namespace impl

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


} // namespace yoga

#endif

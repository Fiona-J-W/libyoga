#ifndef YOGA_MACROS_HPP
#define YOGA_MACROS_HPP


#include "print.hpp"

#ifdef YOGA_NO_LOGGING

#define YOGA_FATAL(...) do{}while(false)
#define YOGA_ERROR(...) do{}while(false)
#define YOGA_WARN(...)  do{}while(false)
#define YOGA_INFO(...)  do{}while(false)
#define YOGA_DEBUG(...) do{}while(false)
#define YOGA_TRACE(...) do{}while(false)

#define YOGA_FATALF(...) do{}while(false)
#define YOGA_ERRORF(...) do{}while(false)
#define YOGA_WARNF(...)  do{}while(false)
#define YOGA_INFOF(...)  do{}while(false)
#define YOGA_DEBUGF(...) do{}while(false)
#define YOGA_TRACEF(...) do{}while(false)

#define YOGA_TRACEPOINT do{}while(false)

#ifdef YOGA_USE_UNCLEAN_MACROS
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
#endif

#else // NOLOGGING

#define YOGA_FATAL(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define YOGA_ERROR(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define YOGA_WARN(...)  ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define YOGA_INFO(...)  ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define YOGA_DEBUG(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define YOGA_TRACE(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define YOGA_FATALF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define YOGA_ERRORF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define YOGA_WARNF(...)  ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define YOGA_INFOF(...)  ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define YOGA_DEBUGF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define YOGA_TRACEF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define YOGA_TRACEPOINT ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, "tracepoint")


#ifdef YOGA_USE_UNCLEAN_MACROS

#define FATAL(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define ERROR(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define WARN(...)  ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define INFO(...)  ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define DEBUG(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define TRACE(...) ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define FATALF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::fatal, __VA_ARGS__)
#define ERRORF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::error, __VA_ARGS__)
#define WARNF(...)  ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::warn,  __VA_ARGS__)
#define INFOF(...)  ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::info,  __VA_ARGS__)
#define DEBUGF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::debug, __VA_ARGS__)
#define TRACEF(...) ::yoga::impl::logf({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, __VA_ARGS__)

#define TRACEPOINT ::yoga::impl::log({__FILE__, __PRETTY_FUNCTION__, __LINE__},\
		::yoga::priority::trace, "tracepoint")

#endif // unclean macros

#endif // no logging

#endif

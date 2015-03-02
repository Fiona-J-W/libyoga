#ifndef YOGA_OVERLOAD_RESOLVER_HPP
#define YOGA_OVERLOAD_RESOLVER_HPP

#include <utility>

#define YOGA_RESOLVE_OVERLOAD(...) \
	[](auto&&...args)->decltype(auto){return (__VA_ARGS__)(std::forward<decltype(args)>(args)...);}

#endif

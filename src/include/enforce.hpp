#ifndef YOGA_ENFORCE_HPP
#define YOGA_ENFORCE_HPP

#include <cassert>
#include <stdexcept>
#include <string>

namespace yoga {

template<typename Exception = std::runtime_error>
void enforce(bool b, const char* msg) {
	assert(msg);
	if (!b) {
		throw Exception{msg};
	}
}

template<typename Exception = std::runtime_error>
void enforce(bool b, const std::string& msg) {
	enforce<Exception>(b, msg.c_str());
}

template<typename Stream>
void enforce_opened(const Stream& stream, const std::string& filename) {
	enforce(stream.is_open(), "Could not open “" + filename + "”");
}

} // namespace util

#endif

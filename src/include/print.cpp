#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <ios>

#include "print.hpp"

namespace yoga {


namespace settings {

namespace {
bool print_time = false;
bool print_location = false;
priority minimum_priority = priority::info;
std::ofstream logfile;

std::atomic_bool logfile_is_open{false};
std::mutex logfile_mutex;
}

priority get_priority() {return minimum_priority;}
void set_priority(priority p) {minimum_priority = p;}

void set_print_time(bool b) {print_time = b;}
bool get_print_time() {return print_time;}

void set_print_location(bool b) {print_location = b;}
bool get_print_location() {return print_location;}

void set_logfile(const std::string& filename) {
	std::lock_guard<std::mutex> guard{logfile_mutex};
	if(logfile.is_open()) {
		logfile.close();
	}
	if(filename.empty()) {
		logfile.open(filename, std::ios_base::out | std::ios_base::app);
		logfile_is_open = logfile.is_open();
	} else {
		logfile_is_open = false;
	}
}
} // namespace settings

void print_priority(std::ostream& stream, priority p) {
	switch(p) {
		case priority::fatal:
			stream << "[Fatal]"; break;
		case priority::error:
			stream << "[Error]"; break;
		case priority::warn:
			stream << "[Warn ]"; break;
		case priority::info:
			stream << "[Info ]"; break;
		case priority::debug:
			stream << "[Debug]"; break;
		case priority::trace:
			stream << "[Trace]"; break;
	}
}

void print_time(std::ostream& stream,
		const std::chrono::time_point<std::chrono::system_clock>& time_point) {
	stream << "[" << time_point.time_since_epoch().count() << "]";
}

void print_location(std::ostream& stream, const location& loc) {
	stream << "[“" << loc.file << "”, " << loc.line << ": " << loc.function << "]";
}

std::tuple<std::string, std::string> get_terminal_format(priority p) {
	switch (p) {
		case priority::fatal:
			return std::make_tuple("\033[1;31m", "\033[0m");
		case priority::error:
			return std::make_tuple("\033[31m", "\033[0m");
		case priority::warn:
			return std::make_tuple("\033[33m", "\033[0m");
		case priority::info:
		case priority::debug:
		case priority::trace:
			return std::make_tuple("", "");
	}
}

static void log_to_terminal(const location& loc, priority p,
		const std::chrono::time_point<std::chrono::system_clock>& timePoint,
		const std::string msg) {
	std::stringstream stream;
	
	if(settings::get_print_time()) {
		print_time(stream, timePoint);
	}
	
	std::string term_Format_Open, term_Format_Close;
	std::tie(term_Format_Open, term_Format_Close) = get_terminal_format(p);
	
	stream << term_Format_Open;
	print_priority(stream, p);
	stream <<term_Format_Close;
	
	if(settings::get_print_location()) {
		print_location(stream, loc);
	}
	
	stream << ": ";
	
	stream << term_Format_Open;
	stream << msg;
	stream << term_Format_Close;
	
	stream.put('\n');
	
	if (p < priority::warn) {
		static std::mutex cout_mutex;
		{
			std::lock_guard<std::mutex> guard{cout_mutex};
			std::cout << stream.str();
		}
	} else {
		static std::mutex cerr_mutex;
		{
			std::lock_guard<std::mutex> guard{cerr_mutex};
			std::cerr << stream.str();
		}
	}
}

static void log_to_file(const location& loc, priority p,
		const std::chrono::time_point<std::chrono::system_clock>& timePoint,
		const std::string& msg) {
	if(!settings::logfile_is_open) {
		return;
	}
	std::stringstream stream;
	print_time(stream, timePoint);
	stream << ' ';
	print_priority(stream, p);
	
	if(settings::get_print_location()) {
		stream << ' ';
		print_location(stream, loc);
	}
	
	stream << ": " << msg << '\n';
	{
		std::lock_guard<std::mutex> guard{settings::logfile_mutex};
		if(!settings::logfile_is_open) {
			return;
		}
		settings::logfile << stream.str() << std::flush;
	}
}

namespace impl {

void log(const location& loc, priority p, const std::string msg) {
	auto time =std::chrono::system_clock::now();
	
	log_to_terminal(loc, p, time, msg);
	log_to_file(loc, p, time, msg);
}

} // namespace impl

} // namespace yoga

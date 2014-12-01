#ifndef YOGA_FILE_HPP
#define YOGA_FILE_HPP

#include <fstream>
#include <istream>
#include <string>
#include <iterator>

namespace yoga {

class lines_range;

class lines_iterator {
public:
	lines_iterator() = default;
	lines_iterator(lines_range& range): m_range{&range} {}
	inline lines_iterator& operator++();
	inline const std::string& operator*() const;
	inline const std::string& operator->() const;
	
	friend bool operator==(const lines_iterator& lhs, const lines_iterator& rhs) {return lhs.m_range == rhs.m_range;}
	friend bool operator!=(const lines_iterator& lhs, const lines_iterator& rhs) {return !(lhs == rhs);}
private:
	lines_range* m_range = nullptr;
};

class lines_range {
public:
	lines_range(const std::string& path): m_stream{path} {}
	
	// sadly this doesn't work by now because currently (2014) libstdc++ doesn't
	// support moves for streams:
	//lines_range(std::ifstream&& stream): m_stream{std::move(stream)} {}
	
	bool next_line() {return std::getline(m_stream, m_line);}
	const std::string& line() const {return m_line;}

	auto begin() {return lines_iterator{*this};}
	auto end() const {return lines_iterator{};}
private:
	std::string m_line;
	std::ifstream m_stream;
};

lines_iterator& lines_iterator::operator++() {
	m_range->next_line() || (m_range = nullptr);
	return *this;
}
const std::string& lines_iterator::operator*() const {
	return m_range->line();
}
const std::string& lines_iterator::operator->() const {
	return operator*();
}


} // namespace yoga

/*
void test() {
	for(auto& line: yoga::lines_range{"foo"}) {
		(void) line;
	}
}
*/

#endif

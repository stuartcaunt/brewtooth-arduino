#include "StringFunctions.h"
#include <algorithm>
#include <cstdarg>

void erase_each_and_every(std::string & input, const std::string & pattern) {
	int length = pattern.size();
	for (int i = 0; i < length; i++) {
		replace_all(input, pattern.substr(i, 1), "");
	}
}

std::string to_lower(const std::string & input) {
	std::string lower = input;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	
	return lower;
}


std::string replace_all(std::string & input, const std::string & find, const std::string & replace) {
	size_t pos = 0;
	while ((pos = input.find(find, pos)) != std::string::npos) {
		input.replace(pos, find.length(), replace);
		pos += replace.length();
	}
	
	return input;
}

std::string & ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string & rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string & trim(std::string &s) {
	return ltrim(rtrim(s));
}

bool startsWith(const std::string & input, const std::string & start) {
	size_t pos = input.find(start);
	
	return (pos == 0);
}

bool endsWith(const std::string & input, const std::string & end) {
	size_t pos = input.find(end);

	return ((pos + end.length()) == input.length());
}

std::vector<std::string> split(const std::string & input, const std::string & separator) {
	size_t i = 0;
	size_t nextI;
	size_t size = input.length();
	std::vector<std::string> tokens;
	while (i < size) {
		nextI = input.find(separator.c_str(), i);
		
		if (nextI == std::string::npos) {
			tokens.push_back(input.substr(i));
			i = size;
		
		} else {
			tokens.push_back(input.substr(i, nextI - i));
			i = nextI + 1;
		}
	}

	return tokens;
}


std::string format(const std::string & format, ...) {
	int size = 100;
	std::string str;
	va_list ap;
	
	while (1) {
		str.resize(size);
		va_start(ap, format);
		
		int n = vsnprintf((char *)str.c_str(), size, format.c_str(), ap);
		
		va_end(ap);
		
		if (n > -1 && n < size) {
			str.resize(n);
			return str;
		}
		
		if (n > -1) {
			size = n + 1;
		} else {
			size *= 2;
		}
	}
	
	return str;	
}

#ifndef STRINGFUNCTIONS_H
#define STRINGFUNCTIONS_H

#include <string>
#include <vector>

void erase_each_and_every(std::string & input, const std::string & pattern);
std::string to_lower(const std::string & input);
std::string replace_all(std::string & input, const std::string & find, const std::string & replace);

std::string & ltrim(std::string &s);
std::string & rtrim(std::string &s);
std::string & trim(std::string &s);

bool startsWith(const std::string & input, const std::string & start);
bool endsWith(const std::string & input, const std::string & end);

std::vector<std::string> split(const std::string & input, const std::string & separator);

std::string format(const std::string & format, ...);

#endif /*STRINGFUNCTIONS_H*/

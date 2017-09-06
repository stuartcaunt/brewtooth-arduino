#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>
#include <cstdlib>
#include <stdlib.h>

template<typename T>
struct Converter {
	static T convert(const std::string & value);
};

template<>
inline int Converter<int>::convert(const std::string & value) {
	int result = atoi(value.c_str());
	return result;
}
	
template<>
inline long Converter<long>::convert(const std::string & value) {
	long result = atol(value.c_str());
	return result;
}

template<>
inline float Converter<float>::convert(const std::string & value) {
	float result = atof(value.c_str());
	return result;
}

template<>
inline double Converter<double>::convert(const std::string & value) {
	double result = atof(value.c_str());
	return result;
}

template<>
inline std::string Converter<std::string>::convert(const std::string & value) {
	return value;
}

#endif /*CONVERTER_H*/

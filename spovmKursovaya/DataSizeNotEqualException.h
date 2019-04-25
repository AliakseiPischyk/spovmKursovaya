#pragma once
#include <stdexcept>
#include <string>
class DataSizeNotEqualException : public std::runtime_error {
	std::string msg;
public:
	DataSizeNotEqualException(const std::string& msg) :runtime_error(msg) {};
};
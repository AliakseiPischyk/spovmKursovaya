#pragma once
#include <string>
#include <stdexcept>
class WrongLambdaException :std::runtime_error{
	std::string msg;
public:
	WrongLambdaException(const std::string& msg) :runtime_error(msg) {};
};


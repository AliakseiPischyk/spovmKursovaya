#pragma once
#include "Pseudonyms.h"

template<class data_t, class = is_arithm<data_t>>
class ExponentialEquation {
private:
	data_t interceptY;
	data_t base;
public:
	ExponentialEquation(const data_t base, const data_t interceptY) :interceptY(interceptY), base(base) {};

	ExponentialEquation() = default;

	data_t getInterceptY()const {
		return interceptY;
	}

	data_t getBase()const {
		return base;
	}
};

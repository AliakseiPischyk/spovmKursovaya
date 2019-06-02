#pragma once
#include "Pseudonyms.h"
template<class data_t, class = is_arithm<data_t>>
class HyperbolicEquaion {
private:
	data_t interceptY;
	data_t slope;
public:
	HyperbolicEquaion(const data_t slope, const data_t interceptY) :
		slope(slope), interceptY(interceptY) {};

	HyperbolicEquaion() = default;

	data_t getSlope()const {
		return slope;
	}

	data_t getInterceptY()const {
		return interceptY;
	}
};
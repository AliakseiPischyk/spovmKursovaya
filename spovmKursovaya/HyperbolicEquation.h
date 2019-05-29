#pragma once
#include "Pseudonyms.h"
template<class data_t, class = is_arithm<data_t>>
class HyperbolicEquaion {
private:
	data_t interceptY;
	data_t slope;
public:
	HyperbolicEquaion(const data_t slope, const data_t interceptY) noexcept :
		slope(slope), interceptY(interceptY) {}

	data_t getSlope()const noexcept {
		return slope;
	}

	data_t getInterceptY()const noexcept {
		return interceptY;
	}
};
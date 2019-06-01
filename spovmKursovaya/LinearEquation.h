#pragma once

template<class data_t, class = std::enable_if_t<std::is_arithmetic_v<data_t>>>
class LinearEquation {
private:
	data_t interceptY;
	data_t slope;
public:
	LinearEquation(const data_t slope, const data_t interceptY) :
		slope(slope), interceptY(interceptY) {}

	data_t getSlope()const {
		return slope;
	}

	data_t getInterceptY()const {
		return interceptY;
	}
};
#pragma once

template<class data_t, class = std::enable_if_t<std::is_arithmetic_v<data_t>>>
class LinearEquation {
private:
	data_t interceptY;
	data_t gradient;
public:
	LinearEquation(const data_t gradient, const data_t interceptY) noexcept :
		gradient(gradient), interceptY(interceptY) {}

	data_t getGradient()const noexcept {
		return gradient;
	}

	data_t getInterceptY()const noexcept {
		return interceptY;
	}
};
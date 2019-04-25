#pragma once

template<class data_t, class = std::enable_if_t<std::is_arithmetic_v<data_t>>>
class PowerEquation {
private:
	data_t gradient;
	data_t power;

public:
	PowerEquation(const data_t gradient, const data_t power) :gradient(gradient), power(power) {};

	data_t getGradient()const noexcept {
		return gradient;
	}

	data_t getPower()const noexcept {
		return power;
	}

	/*data_t calc(const data_t x)const noexcept {
		return gradient * (pow(x, power));
	}*/

	static data_t calc(const data_t x,
		const data_t power,
		const data_t gradient)noexcept {
		return gradient * (pow(x, power));
	}
};
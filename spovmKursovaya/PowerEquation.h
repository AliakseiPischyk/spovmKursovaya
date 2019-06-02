#pragma once

template<class data_t, class = std::enable_if_t<std::is_arithmetic_v<data_t>>>
class PowerEquation {
private:
	data_t gradient;
	data_t power;

public:
	PowerEquation(const data_t gradient, const data_t power) :
		gradient(gradient), power(power) {};

	PowerEquation() = default;

	data_t getGradient()const {
		return gradient;
	}

	data_t getPower()const {
		return power;
	}

	static data_t calculate(const data_t x,
		const data_t power, const data_t gradient) {
		return gradient * (pow(x, power));
	}
};
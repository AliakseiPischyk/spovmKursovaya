#pragma once
#include <vector>
#include "Modifier.h"
#include <numeric>
#include "Pseudonyms.h"
#include "ExponentialEquation.h"

template<class data_t, class = is_floating<data_t>>
class ExponentialEquationCalculator
{
public:
	static ExponentialEquation<data_t>
		calculate(const std::vector<data_t>& dependent,
			const std::vector<data_t>& independent) {

		if (dependent.size() != independent.size()) {
			throw DataSizeNotEqualException(
				"Error in Exponential regression. Data size not equal");
		}

		const std::vector<data_t> lnDependent = Modifier::applyFunc<data_t>(
			dependent.cbegin(), dependent.cend(),
			std::function<data_t(data_t)>([](data_t val) { return log(val); }));

		constexpr data_t ZERO = 0;
		const data_t innerProdLnDepIndep = std::inner_product(
			independent.cbegin(), independent.cend(),
			lnDependent.cbegin(),
			ZERO);

		const data_t sumIndependent = std::accumulate(
			independent.cbegin(), independent.cend(),
			ZERO);

		const data_t sumLnDependent = std::accumulate(
			lnDependent.cbegin(), lnDependent.cend(),
			ZERO);

		const std::vector<data_t> squaresIndependent = Modifier::applyFunc<data_t>(
			independent.cbegin(), independent.cend(),
			std::function<data_t(data_t)>([](data_t value) { return pow(value, 2); }));

		const data_t sumSquaresIndep = std::accumulate(
			squaresIndependent.cbegin(), squaresIndependent.cend(),
			ZERO);

		const size_t dataSize = independent.size();
		const data_t base = exp((dataSize*innerProdLnDepIndep - sumIndependent * sumLnDependent)
			/ ((dataSize*sumSquaresIndep) - pow(sumIndependent, 2)));

		const data_t interceptY = exp((sumLnDependent - log(base)*sumIndependent) / dataSize);

		return ExponentialEquation<data_t>(base, interceptY);
	}
};


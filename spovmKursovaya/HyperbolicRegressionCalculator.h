#pragma once

#include "Pseudonyms.h"
#include "Modifier.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include "HyperbolicEquation.h"

template<class data_t, class = is_floating<data_t>>
class HyperbolicRegressionCalculator {
public:
	static HyperbolicEquaion<data_t> calculate(
		const std::vector<data_t>& dependent,
		const std::vector<data_t>& independent) {

		constexpr data_t ZERO = 0;
		const data_t sumDividedDepByIndep = std::inner_product(
			dependent.cbegin(), dependent.cend(),
			independent.cbegin(),
			ZERO,
			std::plus<data_t>(), std::divides<data_t>());

		const std::vector<data_t> divided1ByIndep = Modifier::applyFunc<data_t>(
			independent.cbegin(), independent.cend(),
			std::function<data_t(data_t)>([](data_t value) { return 1 / value; }));

		const data_t sumDivided1ByIndep = std::accumulate(
			divided1ByIndep.cbegin(), divided1ByIndep.cend(),
			ZERO);

		const data_t sumDivided1BySuqareIndep = std::accumulate(
			divided1ByIndep.cbegin(), divided1ByIndep.cend(),
			ZERO,
			[](data_t currSum, data_t value) { return currSum + (value*value); });

		const data_t sumDep = std::accumulate(dependent.cbegin(),
			dependent.cend(),
			ZERO);

		const size_t dataSize = independent.size();
		const data_t slope = (dataSize*sumDividedDepByIndep - sumDivided1ByIndep * sumDep)
			/ (dataSize*sumDivided1BySuqareIndep - pow(sumDivided1ByIndep, 2));

		const data_t interceptY = (sumDep - slope * sumDivided1ByIndep) / dataSize;

		return HyperbolicEquaion<data_t>(slope, interceptY);
	}
};

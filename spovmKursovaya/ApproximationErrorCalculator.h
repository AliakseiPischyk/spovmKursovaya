#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "Pseudonyms.h"
#include "DataSizeNotEqualException.h"

template<class data_t, class = is_arithm<data_t>>
class ApproximationErrorCalculator
{
public:
	template <class ... funcArgTypes, class ... funcArgValues>
	static data_t calculate(
		const std::vector<data_t>& depVars,
		const std::vector<data_t>& indepVars,
		const std::function <data_t(funcArgTypes...)>& calcFunc,
		const funcArgValues ... funcArgs) {

		if (depVars.size() != indepVars.size()) {
			throw DataSizeNotEqualException("kek");
		}

		std::vector<data_t> deviations;
		const size_t dataSize = indepVars.size();
		deviations.resize(dataSize);

		size_t currPos = 0;
		std::transform(
			indepVars.cbegin(),
			indepVars.cend(),
			deviations.begin(),
			[&depVars, &currPos, &calcFunc, &funcArgs...](data_t indepVal) {
			return abs(static_cast<data_t>(1) - (calcFunc(indepVal, funcArgs...) / depVars[currPos++]));
		});

		const data_t avarageDeviation = std::accumulate(
			deviations.cbegin(),
			deviations.cend(),
			static_cast<data_t>(0))
			/ static_cast<data_t>(dataSize);

		return avarageDeviation;
	}

	static data_t calcRSS(static LinearEquation<data_t>& equation) {

	}
};


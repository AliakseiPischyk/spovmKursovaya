#pragma once
#include "PowerEquation.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <optional>

template<class data_t, class = std::enable_if_t<std::is_floating_point_v<data_t>>>
class PowerRegressionCalculator {
private:
	static data_t calcGradient(const data_t sumLnDep,
		const data_t sumLnIndep,
		const data_t power,
		const size_t dataSize) {
		const data_t gradient = exp((sumLnDep - power * sumLnIndep) / dataSize);
		return gradient;
	}

	static data_t calcPower(
		const std::vector<data_t>& depLnVars,
		const std::vector<data_t>& indepLnVars,
		const data_t sumLnDep,
		const data_t sumLnIndep,
		const std::function<data_t(data_t, data_t)>& accumulator = std::plus<data_t>(),
		const std::function<data_t(data_t, data_t)>& product = std::multiplies<data_t>()) {

		const data_t innerProdLnDepLnIndep = inner_product(depLnVars.cbegin(),
			depLnVars.cend(),
			indepLnVars.cbegin(),
			static_cast<data_t>(0),
			accumulator,
			product);

		const data_t prodOfSumLnDepAndSumLnIndep = sumLnDep * sumLnIndep;

		const data_t squareSumLnIndep = pow(sumLnIndep, 2);

		const data_t sumOfLnLnIndep = inner_product(indepLnVars.cbegin(),
			indepLnVars.cend(),
			indepLnVars.cbegin(),
			static_cast<data_t>(0),
			accumulator,
			product);

		const size_t dataSize = depLnVars.size();
		const data_t power = (dataSize * innerProdLnDepLnIndep - prodOfSumLnDepAndSumLnIndep)
			/ (dataSize * sumOfLnLnIndep - squareSumLnIndep);
		return power;
	}

	static void applyFunc(const std::vector<data_t>& dataVec,
		std::vector<data_t>& result,
		const std::function<data_t(data_t)>& modifier,
		const std::optional<size_t>& begin = std::nullopt,
		const std::optional<size_t>& end = std::nullopt) {

		const size_t beginVal = begin.has_value() ? begin.value() : 0;
		const size_t endVal = end.has_value() ? end.value() : dataVec.size();

		result.resize(endVal - beginVal);

		std::transform(
			dataVec.cbegin() + beginVal,
			dataVec.cbegin() + endVal,
			result.begin(),
			modifier);
	}

public:
	static PowerEquation<data_t> calculate(std::vector<data_t>&	dependentData,
		std::vector<data_t>& independentData,
		const std::function<data_t(data_t, data_t)>& accumulator = std::plus<data_t>(),
		const std::function<data_t(data_t, data_t)>& product = std::multiplies<data_t>()) {

		using namespace std;

		if (dependentData.size() != independentData.size()) {
			throw DataSizeNotEqualException("loh");
		}

		vector<data_t> depLnVars;
		applyFunc(
			dependentData,
			depLnVars, 
			[](data_t val) { return log(val); });

		vector<data_t> indepLnVars;
		applyFunc(
			independentData, 
			indepLnVars,
			[](data_t val) { return log(val); });

		const data_t sumLnDep = accumulate(
			depLnVars.cbegin(),
			depLnVars.cend(),
			static_cast<data_t>(0));

		const data_t sumLnIndep = accumulate(
			indepLnVars.cbegin(),
			indepLnVars.cend(),
			static_cast<data_t>(0));

		const data_t power = calcPower(
			depLnVars, 
			indepLnVars, 
			sumLnDep,
			sumLnIndep);

		const data_t gradient = calcGradient(
			sumLnDep,
			sumLnIndep, 
			power,
			dependentData.size());

		const data_t approximationError = ApproximationErrorCalculator<data_t>::calc(
			dependentData,
			independentData, 
			std::function<data_t(data_t,data_t,data_t)>(PowerEquation<data_t>::calc),
				power,
				gradient);

		return PowerEquation<data_t>(gradient, power);
	}
};
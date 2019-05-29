#pragma once
#include "PowerEquation.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <optional>
#include "Modifier.h"
#include "Pseudonyms.h"

template<class data_t, class = is_floating<data_t>>
class PowerRegressionCalculator {
private:
	static data_t calcPower(
		const std::vector<data_t>& lnDependent,
		const std::vector<data_t>& lnIndependent,
		const data_t sumLnDep,
		const data_t sumLnIndep) {

		constexpr data_t ZERO = 0;

		const data_t innerProdLnDepLnIndep = inner_product(
			lnDependent.cbegin(), lnDependent.cend(),
			lnIndependent.cbegin(),
			ZERO);

		const data_t sumOfSquaredLnIndep = inner_product(
			lnIndependent.cbegin(),	lnIndependent.cend(),
			lnIndependent.cbegin(),
			ZERO);

		const size_t dataSize = lnDependent.size();
		const data_t power = (dataSize * innerProdLnDepLnIndep - sumLnDep * sumLnIndep)
			/ (dataSize * sumOfSquaredLnIndep - pow(sumLnIndep, 2));
		return power;
	}

public:
	static PowerEquation<data_t> calculate(
		const std::vector<data_t>& dependent,
		const std::vector<data_t>& independent) {

		using namespace std;

		if (dependent.size() != independent.size()) {
			throw DataSizeNotEqualException("Power regression. Data size nor equal");
		}

		constexpr data_t ZERO = 0;
		vector<data_t> lnDependent = Modifier::applyFunc<data_t>(
			dependent.cbegin(), dependent.cend(),
			std::function<data_t(data_t)>([](data_t val) { return log(val); }));

		vector<data_t> lnIndependent = Modifier::applyFunc<data_t>
			(independent.cbegin(), independent.cend(),
				std::function<data_t(data_t)>([](data_t val) { return log(val); }));

		const data_t sumLnDependent = accumulate(
			lnDependent.cbegin(), lnDependent.cend(),
			ZERO);

		const data_t sumLnIndependent = accumulate(
			lnIndependent.cbegin(), lnIndependent.cend(),
			ZERO);

		const data_t power = calcPower(lnDependent, lnIndependent, sumLnDependent, sumLnIndependent);

		const data_t gradient = exp((sumLnDependent - power * sumLnIndependent) / independent.size());

		const data_t approximationError = ApproximationErrorCalculator<data_t>::calculate(
			dependent,
			independent,
			std::function<data_t(data_t, data_t, data_t)>(PowerEquation<data_t>::calculate),
			power,
			gradient);

		return PowerEquation<data_t>(gradient, power);
	}
};
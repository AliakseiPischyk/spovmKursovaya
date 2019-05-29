#pragma once

#include "LinearEquation.h"
#include "ApproximationErrorCalculator.h"
#include "DataSizeNotEqualException.h"
#include "Pseudonyms.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>

template<class data_t, class = is_floating<data_t>>
class LinearRegressionCalculator {
public:

	static __forceinline data_t calcInterceptY(
		const data_t gradient,
		const data_t sumDependentVars,
		const data_t sumIndependentVars,
		const size_t numElems) {

		return (sumDependentVars - gradient * sumIndependentVars) / numElems;
	}

public:
	static LinearEquation<data_t> calculate(
		const std::vector<data_t>& dependent,
		const std::vector<data_t>& independent) {

		if (independent.size() != dependent.size()) {
			throw DataSizeNotEqualException("Linear regression. Data size not equal");
		}

		constexpr data_t ZERO = 0;
		const data_t sumDependentVars = std::accumulate(
			dependent.begin(), dependent.end(),
			ZERO);

		const data_t sumIndependentVars = std::accumulate(
			independent.begin(),
			independent.end(),
			ZERO);

		const data_t sumSquaresIndep = std::inner_product(
			independent.begin(),
			independent.end(),
			independent.begin(),
			ZERO);

		const data_t innerProdDepIndep = std::inner_product(
			dependent.begin(),
			dependent.end(),
			independent.begin(),
			ZERO);

		const size_t dataSize = independent.size();

		const data_t gradient = (sumDependentVars * sumIndependentVars - dataSize * innerProdDepIndep)
			/ (pow(sumIndependentVars, 2) - dataSize * sumSquaresIndep);

		const data_t interceptY = calcInterceptY(gradient, sumDependentVars, sumIndependentVars, dataSize);

		return	LinearEquation<data_t>(gradient, interceptY);
	}
};
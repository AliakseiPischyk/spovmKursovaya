#pragma once

#include "LinearEquation.h"
#include "ApproximationErrorCalculator.h"
#include "DataSizeNotEqualException.h"
	
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>

template<class data_t, class = std::enable_if_t<std::is_floating_point_v<data_t>>>
class LinearRegressionCalculator {
private:
	static data_t calcGradient(
		const data_t sumDependentVars,
		const data_t sumindependentVars,
		const size_t numElems,
		const data_t innerProdDepIndep,
		const data_t innerProdDepDep) {
		return (numElems * innerProdDepIndep - sumDependentVars * sumindependentVars) / (numElems * innerProdDepDep - pow(sumDependentVars, 2));
	}

	static data_t calcInterceptY(
		const data_t sumDependentVars,
		const data_t sumindependentVars,
		const data_t numElems,
		const data_t gradient) {
		return (sumindependentVars - gradient * sumDependentVars) / numElems;
	}

public:
	static LinearEquation<data_t> calcRegressionLinearEquation(
		const std::vector<data_t>& dependentVars,
		const std::vector<data_t>& independentVars,
		const std::function<data_t(data_t, data_t)>& accumulator = std::plus<data_t>(),
		const std::function<data_t(data_t, data_t)>& product = std::multiplies<data_t>()) {


		if (independentVars.size() != dependentVars.size()) {
			throw DataSizeNotEqualException("infO");
		}
		
		const data_t sumDependentVars = std::accumulate(
			dependentVars.begin(), 
			dependentVars.end(), 
			static_cast<data_t>(0));

		const data_t sumindependentVars = std::accumulate(
			independentVars.begin(), 
			independentVars.end(),
			static_cast<data_t>(0));

		const data_t innerProdDepDep = std::inner_product(
			dependentVars.begin(),
			dependentVars.end(),
			dependentVars.begin(),
			static_cast<data_t>(0),
			accumulator, 
			product);

		const data_t innerProdDepIndep = std::inner_product(
			dependentVars.begin(), 
			dependentVars.end(), 
			independentVars.begin(), 
			static_cast<data_t>(0),
			accumulator, 
			product);

		const size_t dataSize = independentVars.size();
		const data_t gradient = calcGradient(
			sumDependentVars,
			sumindependentVars,
			dataSize, 
			innerProdDepIndep,
			innerProdDepDep);

		const data_t interceptY = calcInterceptY(
			sumDependentVars, 
			sumindependentVars,
			dataSize,
			gradient);

		return	LinearEquation<data_t>(gradient, interceptY);
	}
};
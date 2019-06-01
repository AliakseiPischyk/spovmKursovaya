#pragma once
#include <vector>
#include <numeric>
#include <functional>
#include <optional>
#include "Modifier.h"
#include "LinearRegressionCalculator.h"
#include "LinearEquation.h"
#include "ThreadPool.h"
#include "Generator.h"
#include <algorithm>

enum RegulatorType {
	Ridge,
	LASSO,
	ElasticNet
};

template<class data_t>
class Regularizator {
private:

	static __forceinline data_t calculateGradient(
		const data_t innerProdDepIndep,
		const data_t sumSquaresIndep,
		const data_t lambdaRidge,
		const data_t lambdaLasso) {

		const data_t gradientGreater0 = (innerProdDepIndep - (lambdaLasso / 2))
			/ (sumSquaresIndep + lambdaRidge);

		const data_t gradientLessEqual0 = ((lambdaLasso / 2) - innerProdDepIndep)
			/ (sumSquaresIndep + lambdaRidge);

		return gradientGreater0 > 0 ? gradientGreater0 : gradientLessEqual0;
	}

	static std::vector<data_t> calculatePossibleGradients(
		const data_t innerProdDepIndep,
		const data_t sumSquaresIndep,
		const std::vector<data_t> lambdasRidge,
		const std::vector<data_t> lambdasLasso,
		const RegulatorType regulatorType) {

		std::vector<data_t> possibleGradients;
		const size_t numSubstitutionAttempts = lambdasLasso.size();

		possibleGradients.reserve(regulatorType ==
			RegulatorType::ElasticNet ?
			static_cast<size_t>(pow(numSubstitutionAttempts, 2)) :
			numSubstitutionAttempts);

		switch (regulatorType) {
		case RegulatorType::ElasticNet: {
			for (data_t lambdaRidge : lambdasRidge) {
				for (data_t lambdaLasso : lambdasLasso) {
					possibleGradients.push_back(
						calculateGradient(
							innerProdDepIndep,
							sumSquaresIndep,
							lambdaRidge,
							lambdaLasso));
				}
			}
		}break;

		case RegulatorType::LASSO:
		case RegulatorType::Ridge: {
			for (size_t iAttempt = 0; iAttempt < numSubstitutionAttempts; iAttempt++) {
				possibleGradients.push_back(
					calculateGradient(
						innerProdDepIndep,
						sumSquaresIndep,
						lambdasRidge[iAttempt],
						lambdasLasso[iAttempt]));
			}
		}break;
		}
		return possibleGradients;
	}

	static std::vector<LinearEquation<data_t>> makeEquations(
		const std::vector<data_t>& gradients,
		const data_t sumDependent,
		const data_t sumIndependent,
		const size_t dataSize) {

		const std::vector<data_t> interceptsY = Modifier::applyFunc<data_t>(
			gradients.cbegin(), gradients.cend(),
			std::function < data_t(data_t, data_t, data_t, data_t)>(
				LinearRegressionCalculator<data_t>::calcInterceptY),
			sumDependent, sumIndependent, dataSize);

		std::vector<LinearEquation<data_t>> linearEquations;
		const size_t numGradients = gradients.size();
		linearEquations.reserve(numGradients);

		for (size_t i = 0; i < numGradients; i++) {
			linearEquations.push_back(
				LinearEquation<data_t>(gradients[i], interceptsY[i]));
		}

		return linearEquations;
	}

	static __forceinline data_t RSS(
		const LinearEquation<data_t>& equation,
		const std::vector<data_t>& dependent,
		const std::vector<data_t>& independent) {

		std::vector<data_t> actualResults(independent.size());

		const data_t slope = equation.getSlope();
		const data_t interceptY = equation.getInterceptY();
		std::transform(
			independent.cbegin(), independent.cend(),
			actualResults.begin(),
			[slope, interceptY](data_t value) {return value * slope + interceptY; });

		return std::inner_product(
			dependent.cbegin(), dependent.cend(),
			actualResults.cbegin(),
			static_cast<data_t>(0),
			std::plus<data_t>(),
			[](data_t realY, data_t expectedY) {return pow(realY - expectedY, 2); });
	}

	static __forceinline data_t calcApproximationErrorWithPenalty(
		const LinearEquation<data_t>& equation,
		const std::vector<data_t>& dependent,
		const std::vector<data_t>& independent,
		const data_t lambdaRidge,
		const data_t lambdaLasso) {

		const data_t slope = equation.getSlope();
		return RSS(equation, dependent, independent)
			+ lambdaLasso * abs(slope)
			+ lambdaRidge * (pow(slope, 2));

	}

	static LinearEquation<data_t> findBestLinearEquation(
		const std::vector<LinearEquation<data_t>>& equations,
		const std::vector<data_t>& testDependent,
		const std::vector<data_t>& testIndependent,
		const std::vector<data_t>& lambdasRidge,
		const std::vector<data_t>& lambdasLasso,
		const RegulatorType regulator) {

		const size_t dataSize = lambdasLasso.size();
		data_t currErr = std::numeric_limits<data_t>::max();
		data_t lastErr = currErr;
		size_t bestEquationIdx = 0;

		switch (regulator) {
		case RegulatorType::ElasticNet: {
			const size_t lambdasRidgeSize = lambdasRidge.size();
			const size_t lambdasLassoSize = lambdasLasso.size();

			for (size_t iLambdaRidge = 0; iLambdaRidge < lambdasRidgeSize; iLambdaRidge++) {
				for (size_t jLambdaLasso = 0; jLambdaLasso < lambdasLassoSize; jLambdaLasso++) {
					const size_t equationIdx = iLambdaRidge * lambdasLassoSize + jLambdaLasso;
					lastErr = calcApproximationErrorWithPenalty(equations[equationIdx],
						testDependent, testIndependent,
						lambdasRidge[iLambdaRidge], lambdasLasso[jLambdaLasso]);

					if (lastErr < currErr) {
						bestEquationIdx = equationIdx;
						currErr = lastErr;
					}
				}
			}

		}break;

		case RegulatorType::LASSO:
		case RegulatorType::Ridge: {
			for (size_t i = 0; i < dataSize; i++) {
				lastErr = calcApproximationErrorWithPenalty(equations[i],
					testDependent, testIndependent,
					lambdasRidge[i], lambdasLasso[i]);

				if (lastErr < currErr) {
					bestEquationIdx = i;
					currErr = lastErr;
				}
			}
		}
		};

		return equations[bestEquationIdx];
	}

	public:

	static LinearEquation<data_t> calculate(
		const std::vector<data_t>& dep,
		const std::vector<data_t>& indep,
		const std::vector<data_t>& testDep,
		const std::vector<data_t>& testIndep,
		const RegulatorType regulatorType,
		const data_t minLambda,
		const data_t step,
		const data_t maxLambda) {

		constexpr data_t ZERO = 0;

		const data_t innerProdDepIndep = std::inner_product(
			dep.cbegin(), dep.cend(),
			indep.cbegin(),
			ZERO);

		const data_t sumSquaresIndep = std::inner_product(
			indep.begin(), indep.cend(),
			indep.cbegin(),
			ZERO);

		const data_t sumDep = std::accumulate(
			dep.cbegin(), dep.cend(),
			ZERO);

		const data_t sumIndep = std::accumulate(
			indep.cbegin(),	indep.cend(),
			ZERO);

		const std::vector<data_t> lambdasLasso =
			regulatorType == RegulatorType::LASSO || regulatorType == RegulatorType::ElasticNet ?
			Generator::generateSequence<data_t>(minLambda, step, maxLambda) :
			Generator::generateValues<data_t>(static_cast<size_t>((maxLambda - minLambda) / step), 0);

		const std::vector<data_t> lambdasRidge =
			regulatorType == RegulatorType::Ridge || regulatorType == RegulatorType::ElasticNet ?
			Generator::generateSequence<data_t>(minLambda, step, maxLambda) :
			Generator::generateValues<data_t>(static_cast<size_t>((maxLambda - minLambda) / step), 0);

		const std::vector<data_t> possibleGradients = calculatePossibleGradients(
			innerProdDepIndep, sumSquaresIndep,
			lambdasRidge, lambdasLasso, regulatorType);

		const std::vector<LinearEquation<data_t>> possibleEquations =
			makeEquations(possibleGradients, sumDep, sumIndep, dep.size());

		const LinearEquation<data_t> bestEquation = findBestLinearEquation(
			possibleEquations,
			testDep, testIndep,
			lambdasRidge, lambdasLasso,
			regulatorType);

		return bestEquation;
	}
};

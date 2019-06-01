#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"
#include "MultipleRegressionCalculator.h"
#include <future>
#include "ThreadPool.h"
#include "Regularizator.h"
#include "DecisionTree.h"
#include "ExponentialEquationCalculator.h"
#include "HyperbolicRegressionCalculator.h"

#include <thread>

int main() {
	using TYPE = float_t;

	std::map<std::string, std::vector<TYPE >> myMap = DSVParser< ',', float_t>::read("C:\\Users\\Sergei\\Desktop\\file.txt");

	std::vector<TYPE> depVars, indepVars,trainDep,trainIndep,lassoDep,lassoIndep;

	const auto linear = LinearRegressionCalculator<TYPE>::calculate(depVars, indepVars);
	std::cout <<"linear slope" << linear.getSlope() << '\n';
	std::cout << "Linear intercept "<< linear.getInterceptY()<< '\n';

	const auto powerEqu = PowerRegressionCalculator<TYPE >::calculate(depVars, indepVars);
	std::cout << "power base " <<powerEqu.getGradient()<< '\n';
	std::cout << "power power " <<powerEqu.getPower() << '\n';

	const auto hyperbolic = HyperbolicRegressionCalculator<TYPE>::calculate(depVars, indepVars);
	std::cout << "hyperbolic slope " << hyperbolic.getSlope() << '\n';
	std::cout << "hyperbolic intercept " << hyperbolic.getInterceptY()<< '\n';

	const auto exponential = ExponentialEquationCalculator<TYPE>::calculate(depVars,indepVars);
	std::cout << "exponential base " << exponential.getBase() << '\n';
	std::cout << "exponential intercept " << exponential.getInterceptY() << '\n';

	const auto ridge = Regularizator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::Ridge, -1, 1, 100);
	std::cout << "ridge slope " << ridge.getSlope() << '\n';
	std::cout << "ridge intercept " << ridge.getInterceptY() << '\n';

	const auto lasso = Regularizator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::LASSO, -1, 1, 100);
	std::cout << "lasso slope " << lasso.getSlope() << '\n';
	std::cout << "lasso intercept " << lasso.getInterceptY() << '\n';

	const auto elasticNet = Regularizator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::ElasticNet, -1, 1, 100);
	std::cout << "elasticNet slope " << elasticNet.getSlope() << '\n';
	std::cout << "elasticNet intercept " << elasticNet.getInterceptY() << '\n';

	system("pause");
}

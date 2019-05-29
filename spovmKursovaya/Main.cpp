#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"
#include "MultipleRegressionCalculator.h"
#include <future>
#include "ThreadPool.h"
#include "RidgeRegressionCalculator.h"
#include "DecisionTree.h"
#include "ExponentialEquationCalculator.h"
#include "HyperbolicRegressionCalculator.h"

#include <thread>

int main() {
	using TYPE = float_t;
	std::vector<TYPE> lassoDep{	1,1.5,2,1.5,2,2  };
	std::vector<TYPE> lassoIndep{ 1,1,1,1.5,1.5,2 };

	std::vector<TYPE> trainDep{ 3,3,4,4,-5 };
	std::vector<TYPE> trainIndep{ 5,6,5,6,5 };

	/*std::vector<TYPE> depVars{ 1,2,9,10,10 };
	std::vector<TYPE> indepVars{ 1,2,3,4,5};*/

	std::vector<TYPE> matrDepVars;// { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	std::vector<TYPE> matrIndepVars;// { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	for (int i = 0; i < 1152; i++) {
		matrDepVars.push_back(i);
		matrIndepVars.push_back(i);
	}
	
	std::vector<TYPE> res(1152);

	float* ind = matrIndepVars.data();
	float* dep = matrDepVars.data();
	float* ress = res.data();

	

	Matrix<TYPE*>::fastMult(72, 16, 16, ind, dep, ress);
	//Matrix<TYPE*>::transpose(dep,ress,8,4);

	for (int i = 0; i < 1152; i++) {
		std::cout << *ress++ << " ";
	}
	/*Matrix<TYPE*>::multSSE(dep, ind, ress, 32);
	for (int i = 0; i < 32; i++) {
		std::cout << *ress++<< " ";
	}*/

	/*std::vector<TYPE> depdep = { 83, 71 ,64 ,69 ,69 ,64 ,68 ,59 ,81 ,91 ,57 ,65, 58, 62 };
	std::vector<TYPE> indind = { 183 ,168 ,171 ,178 ,176 ,172 ,165 ,158 ,183 ,182 ,163 ,175 ,164 ,175 };*/

	std::vector<TYPE> depVars = { 183 ,168 ,171 ,178 ,176 ,172 ,165 ,158 ,183 ,182 ,163 ,175 ,164 ,175 };
	std::vector<TYPE> indepVars = { 83, 71 ,64 ,69 ,69 ,64 ,68, 59 ,81 ,91, 57, 65, 58, 62 };
	
	
	std::vector<TYPE> treeDepVars = { 1,2,9,10,10 };
	DecisionTree<TYPE>::partitionPointMinRSS(treeDepVars.cbegin(),treeDepVars.cend());
	DecisionTree<TYPE>::build(treeDepVars.cbegin(),treeDepVars.cend());
	system("pause");
	//exit(0);

	//PowerRegressionCalculator<TYPE >::calculate(depVars, indepVars);

	std::map<std::string, std::vector<TYPE >> myMap = DSVParser< ',', float_t>::read("C:\\Users\\Sergei\\Desktop\\file.txt");

	system("cls");
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

	const auto ridge = RidgeRegressionCalculator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::Ridge, -1, 1, 100);
	std::cout << "ridge slope " << ridge.getSlope() << '\n';
	std::cout << "ridge intercept " << ridge.getInterceptY() << '\n';

	const auto lasso = RidgeRegressionCalculator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::LASSO, -1, 1, 100);
	std::cout << "lasso slope " << lasso.getSlope() << '\n';
	std::cout << "lasso intercept " << lasso.getInterceptY() << '\n';

	const auto elasticNet = RidgeRegressionCalculator<TYPE>::calculate(lassoDep, lassoIndep, trainDep, trainIndep, RegulatorType::ElasticNet, -1, 1, 100);
	std::cout << "elasticNet slope " << elasticNet.getSlope() << '\n';
	std::cout << "elasticNet intercept " << elasticNet.getInterceptY() << '\n';

	system("pause");
}

#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"


int main() {
	
	std::vector<double> depVars{ 183,168, 171, 178 };
	std::vector<double> indepVars{ 83,71, 64, 69 };

	PowerRegressionCalculator<double_t>::calculate(depVars, indepVars);

	//std::map<std::string, std::vector<float_t>> myMap = DSVParser< ',', float_t>::read("C:\\Users\\Sergei\\Desktop\\file.txt");

	//std::cout << LinearRegressionCalculator::calcRegressionLinearEquation(depVars, indepVars).getGradient() << '\n';
	//std::cout << LinearRegressionCalculator::calcRegressionLinearEquation(depVars, indepVars).getInterceptY()<< '\n';
	system("pause");
}

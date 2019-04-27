#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"
#include "MultipleRegressionCalculator.h"


int main() {

	Matrix<float_t*> m;
	
	using TYPE = float_t;
	std::vector<TYPE> depVars{ 183,168, 171, 178 };
	std::vector<TYPE> indepVars{ 83,71, 64, 69 };

	PowerRegressionCalculator<TYPE >::calculate(depVars, indepVars);

	std::map<std::string, std::vector<TYPE >> myMap = DSVParser< ',', float_t>::read("C:\\Users\\Sergei\\Desktop\\file.txt");

	std::cout << LinearRegressionCalculator<TYPE>::calcRegressionLinearEquation(depVars, indepVars).getGradient() << '\n';
	std::cout << LinearRegressionCalculator<TYPE>::calcRegressionLinearEquation(depVars, indepVars).getInterceptY()<< '\n';
	system("pause");
}

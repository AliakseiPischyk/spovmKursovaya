#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"
#include "MultipleRegressionCalculator.h"
#include <future>
#include "TimeUtil.h"
#include "ThreadPool.h"
#include "DecisionTree.h"

#include <thread>

class Logarithmator
{
public:
	static void calc(const std::vector<double> vec,
		std::vector<double>::iterator beg,
		std::vector<double>::iterator end)
	{
		std::transform(beg, end, beg, [](double keks) {
			return log(keks); });
	}
	static int kek() {
		return 5;
	}
};

int main() {

	
	using TYPE = float_t;
	/*std::vector<TYPE> depVars{ 183,168, 171, 178 };
	std::vector<TYPE> indepVars{ 83,71, 64, 69 };*/

	std::vector<TYPE> depVars{ 1,2,9,10,10 };
	std::vector<TYPE> indepVars{ 1,2,3,4,5};
	std::vector<TYPE> rss;
	DecisionTree<TYPE>::partitionPointMinRSS(depVars,0,depVars.size());
	system("pause");
	exit(0);

	PowerRegressionCalculator<TYPE >::calculate(depVars, indepVars);

	std::map<std::string, std::vector<TYPE >> myMap = DSVParser< ',', float_t>::read("C:\\Users\\Sergei\\Desktop\\file.txt");

	std::cout << LinearRegressionCalculator<TYPE>::calcRegressionLinearEquation(depVars, indepVars).getGradient() << '\n';
	std::cout << LinearRegressionCalculator<TYPE>::calcRegressionLinearEquation(depVars, indepVars).getInterceptY()<< '\n';
	system("pause");
}

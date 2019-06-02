#include "DSVParser.h"
#include "LinearRegressionCalculator.h"
#include "PowerRegressionCalculator.h"
#include "MultipleRegressionCalculator.h"
#include <future>
#include <vector>
#include "ThreadPool.h"
#include "Regularizator.h"
#include "StringParser.h"
#include "DecisionTree.h"
#include "ExponentialEquationCalculator.h"
#include "HyperbolicRegressionCalculator.h"
#include "DataSplitter.h"

using T = float_t;

int main(int argc, char* argv[]) {
	using namespace std;
	if (argc < 9) {
		cout << "Wrong args passed." << endl
			<< "1 path to executable." << endl
			<< "2 path to file with CSV data" << endl
			<< "3 dependent variable name" << endl
			<< "4 independent variable name" << endl
			<< "...lambdas for regularizators..." << endl
			<< "5 min" << endl
			<< "6 max" << endl
			<< "7 step" << endl
			<< "8 separator in CSV file" << endl
			<< "9 percent of data which goes as testing (0-100) " << endl;
		exit(0);
	}

	ThreadPool threadPool(8);

	const filesystem::path CSVFile = argv[1];
	ifstream stream(CSVFile);
	vector <string> columnsNames; columnsNames = DSVParser<',', T>::getKeys(stream); stream.close();

	auto csvData = threadPool.enqueue(DSVParser< ',', T>::read, CSVFile).get();

	const vector<T> dependent = move(csvData.find(argv[2]).operator*().second);
	const vector<T> independent = move(csvData.find(argv[3]).operator*().second);

	const T minLambda = StringParser::string_cast<T>(argv[4]);
	const T maxLambda = StringParser::string_cast<T>(argv[5]);
	const T stepLambda = StringParser::string_cast<T>(argv[6]);

	const char DSVSepatator = StringParser::string_cast<char>(argv[7]);

	const size_t percentOfDataForTesting = StringParser::string_cast<size_t>(argv[8]);

	auto[trainingDependent, testingDependent] = DataSplitter::splitDataForTrainingAndTesting<T>(dependent, percentOfDataForTesting);
	auto[trainingIndependent, testingIndependent] = DataSplitter::splitDataForTrainingAndTesting<T>(dependent, percentOfDataForTesting);

	auto ridgeLinearEquationFuture = threadPool.enqueue(Regularizator<T>::calculate, trainingDependent, trainingIndependent,
		testingDependent, testingIndependent, RegulatorType::Ridge, minLambda, stepLambda, maxLambda);
	auto lassoLinearEquationFuture = threadPool.enqueue(Regularizator<T>::calculate, trainingDependent, trainingIndependent,
		testingDependent, testingIndependent, RegulatorType::LASSO, minLambda, stepLambda, maxLambda);
	auto elasticNetLinearEquationFuture = threadPool.enqueue(Regularizator<T>::calculate, trainingDependent, trainingIndependent,
		testingDependent, testingIndependent, RegulatorType::ElasticNet, minLambda, stepLambda, maxLambda);

	///////////////////////////////////////////////////////////////////////////////

	auto linearEquationFuture = threadPool.enqueue(LinearRegressionCalculator<T>::calculate, dependent, independent);
	auto powerEquationFuture = threadPool.enqueue(PowerRegressionCalculator<T >::calculate, dependent, independent);
	auto hyperbolicEquationFuture = threadPool.enqueue(HyperbolicRegressionCalculator<T>::calculate, dependent, independent);
	auto exponentialEquationFuture = threadPool.enqueue(ExponentialEquationCalculator<T>::calculate, dependent, independent);

	cout << "Regression equation coefficients:" << endl;
	const auto linearEquation = linearEquationFuture.get();
	cout << endl << "linear " << endl;
	cout << "slope " << linearEquation.getSlope() << '\n';
	cout << "intercept " << linearEquation.getInterceptY() << '\n';

	const auto powerEquation = powerEquationFuture.get();
	cout << endl << "power " << endl;
	cout << "base " << powerEquation.getGradient() << '\n';
	cout << "power " << powerEquation.getPower() << '\n';

	const auto hyperbolicEquation = hyperbolicEquationFuture.get();
	cout << endl << "hyperbolic " << endl;
	cout << "slope " << hyperbolicEquation.getSlope() << '\n';
	cout << "intercept " << hyperbolicEquation.getInterceptY() << '\n';

	const auto exponentialEquation = exponentialEquationFuture.get();
	cout << endl << "exponential " << endl;
	cout << "exponential base " << exponentialEquation.getBase() << '\n';
	cout << "exponential intercept " << exponentialEquation.getInterceptY() << '\n';

	/////////////////////////////////////////////////////////////////////////////////////////////////

	const auto ridgeLinearEquation = ridgeLinearEquationFuture.get();
	cout << endl << "ridge" << endl;
	cout << "slope " << ridgeLinearEquation.getSlope() << '\n';
	cout << "intercept " << ridgeLinearEquation.getInterceptY() << '\n';

	const auto lassoLinearEquation = lassoLinearEquationFuture.get();
	cout << endl << "LASSO " << endl;
	cout << "slope " << lassoLinearEquation.getSlope() << '\n';
	cout << "intercept " << lassoLinearEquation.getInterceptY() << '\n';

	const auto elasticNetLinearEquation = elasticNetLinearEquationFuture.get();
	cout << endl << "elastic net" << endl;
	cout << "slope " << elasticNetLinearEquation.getSlope() << '\n';
	cout << "intercept " << elasticNetLinearEquation.getInterceptY() << '\n';

	system("pause");
}

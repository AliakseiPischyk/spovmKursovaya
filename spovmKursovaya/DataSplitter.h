#pragma once
#include <vector>
class DataSplitter {
public:
	template<class data_t>
	static std::pair<std::vector<data_t>, std::vector<data_t>> splitDataForTrainingAndTesting(
		const std::vector<data_t> values,
		const size_t howManyPercentGoesToTestingData) {

		const size_t dataSize = values.size();
		std::vector<data_t> training; training.reserve(dataSize);
		std::vector<data_t> testing; testing.reserve(dataSize);

		constexpr size_t MAX_PERCENTS = 100;
		const size_t rangeTesting = howManyPercentGoesToTestingData;
		const size_t rangeTraining = MAX_PERCENTS - rangeTesting;

		size_t currentPos = 0;
		while (currentPos + MAX_PERCENTS < dataSize) {
			for (int i = 0; i < rangeTesting; i++) {
				testing.push_back(values[currentPos + i]);
			}
			currentPos += rangeTesting;
			for (int i = 0; i < rangeTraining; i++) {
				training.push_back(values[currentPos + i]);
			}
			currentPos += rangeTraining;
		}

		const size_t dataSizeMinusTwo = dataSize - 2;
		while (currentPos++ < dataSizeMinusTwo) {
			testing.push_back(values[currentPos++]);
			training.push_back(values[currentPos]);
		}

		return std::make_pair(std::move(training), std::move(testing));
	}
};
#pragma once
#include <vector>
class Generator {
public:
	template<class data_t>
	static std::vector<data_t> generateSequence(const data_t begin,
		const data_t step,
		const data_t end) {

		std::vector<data_t> result(static_cast<size_t>((end - begin) / step));
		data_t currValue = begin;
		std::generate(result.begin(),
			result.end(),
			[&currValue, step]() {return currValue += step; });

		return result;
	}

	template <class data_t>
	static std::vector<data_t> generateValues(const size_t numElems, const data_t value) {
		std::vector<data_t> result(numElems);
		std::fill(result.begin(),
			result.end(),
			value);

		return result;
	}
};
#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <iterator>
#include <cstddef>
#include <optional>
#include <functional>

template<class data_t, class = std::enable_if_t<std::is_floating_point_v<data_t>>>
class DecisionTree
{
public:

	static __forceinline data_t calcAvarage(const std::vector<data_t>& values,
		const std::optional<size_t> begin = std::nullopt,
		const std::optional<size_t> end = std::nullopt) {

		const size_t beginPos = begin.has_value() ? begin.value() : 0;
		const size_t endPos = end.has_value() ? end.value() : values.size();

		return std::accumulate(
			values.cbegin() + beginPos,
			values.cbegin() + endPos,
			static_cast<data_t>(0))
			/ (endPos - beginPos);
	}

	static  data_t calcRSS(const std::vector<data_t>& values,
		const std::optional<size_t> begin = std::nullopt,
		const std::optional<size_t> end = std::nullopt) {

		const size_t beginPos = begin.has_value() ? begin.value() : 0;
		const size_t endPos = end.has_value() ? end.value() : values.size();

		const data_t avarageValue = calcAvarage(values, beginPos, endPos);

		data_t sum = 0;
		std::for_each(
			values.cbegin() + beginPos,
			values.cbegin() + endPos,
			[&sum, avarageValue](data_t val) {sum += pow((val - avarageValue), 2); });

		return sum;
	}

	static void calcRSSForAllGroups(const std::vector<data_t>& values,
		std::vector<data_t>& valuesRSS,
		const size_t begin,
		const size_t end){

		const size_t dataSize = end-begin;
		valuesRSS.reserve(dataSize-1);
		
		for (size_t bound = begin + 1; bound < end; bound++) {
			valuesRSS.push_back(calcRSS(values, begin, bound)
			+calcRSS(values,bound,end));
		}
	}

	static auto partitionPointMinRSS(const std::vector<data_t>& values,
		const size_t begin,
		const size_t end){

		std::vector<data_t> valuesRSS;
		calcRSSForAllGroups(values,valuesRSS,begin,end);
		return std::min_element(valuesRSS.cbegin(),valuesRSS.cend());
	}

public:
	std::unordered_map<Question, SomeWiredNode> map;
};


class Question {

};

class Range {
private:
	std::pair<size_t, size_t> offsetFromBegin;
	//getBegin()const {

	//}
};

class SomeWiredNode {
private:
	Range offset;
	
};


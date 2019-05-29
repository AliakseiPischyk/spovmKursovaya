#pragma once
#include <vector>
#include <functional>
#include <optional>

class Modifier
{
public:
	template <class data_t, class InputIt, class ... funcArgTypes, class ... funcArgs>
	static std::vector<data_t> applyFunc(InputIt first, InputIt last,
		const std::function <data_t(funcArgTypes...)>& func,
		const funcArgs ... funcArgsValues) {

		std::vector<data_t> result;
		result.resize(last - first);

		std::transform(
			first, last,
			result.begin(),
			[&func, &funcArgsValues...](data_t value) {
			return func(value, funcArgsValues...);
		});

		return result;
	}

	template<class data_t>
	static std::vector<data_t> generateSequence(const data_t begin,
		const data_t step,
		const data_t end) {

		std::vector<data_t> result((end - begin) / step);
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


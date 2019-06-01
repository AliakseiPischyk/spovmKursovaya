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
};


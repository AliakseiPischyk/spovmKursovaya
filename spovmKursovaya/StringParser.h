#pragma once
#include <string>
#include <type_traits>

class StringParser {
public:
	template <class return_t>// std::enable_if_t<std::is_fundamental_v<return_t>>>
	static __forceinline return_t string_cast(const std::string& str) {
		std::stringstream ss(str);
		return_t tmp;
		ss >> tmp;
		return tmp;
	}
};

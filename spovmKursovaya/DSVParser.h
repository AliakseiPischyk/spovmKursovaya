#pragma once

#include <map>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <algorithm>

#include "DataSizeNotEqualException.h"

template<char delimeter, class data_t, class = std::enable_if_t<std::is_floating_point_v<data_t>>>
class DSVParser {
private:
	template <class return_t>
	static __forceinline return_t stringCast(const std::string& str) {
		std::stringstream ss(str);
		return_t tmp;
		ss >> tmp;
		return tmp;
	}

	template<class return_t, class = std::enable_if_t < std::is_same_v<return_t, data_t> || std::is_same_v<return_t, std::string>>>
	static std::vector<return_t> parseDSVString(const std::string& strToParse) {
		using namespace std;

		vector<return_t> parsedValues;
		stringstream strToParseStream(strToParse);

		string valueToken;
		while (getline(strToParseStream, valueToken, delimeter)) {
			parsedValues.push_back(stringCast<return_t>(valueToken));
		}

		return parsedValues;
	}

	static std::vector<std::string> getKeys(std::ifstream& fileStream) {
		using namespace std;
		string bufferForKeys;
		fileStream >> bufferForKeys;
		return parseDSVString<string>(bufferForKeys);
	}

	static std::vector<std::vector<data_t>> getValues(std::ifstream& fileStream, const size_t amountOfValuesInColumn) {
		using namespace std;
		vector<vector<data_t>> values;
		values.resize(amountOfValuesInColumn);

		std::string bufferWithValues;
		while (fileStream >> bufferWithValues) {
			vector<data_t> lineOfParsedValues(parseDSVString<data_t>(bufferWithValues));
			for (size_t i_key = 0; i_key < lineOfParsedValues.size(); i_key++) {
				values[i_key].push_back(lineOfParsedValues[i_key]);
			}
		}
		values.shrink_to_fit();
		return values;
	}

	static std::map<std::string, std::vector<data_t>> linkKeysAndValues(std::vector<std::string>& keys, std::vector<std::vector<data_t>>& values) {
		const auto keysSize = keys.size();
		const auto valuesSize = values.size();
		std::map<std::string, std::vector<data_t>> result;

		if (keysSize != valuesSize) {
			throw DataSizeNotEqualException("Amount of keys and values doesn't match");
		}
		else {
			for (size_t i = 0; i < keysSize; i++) {
				result.insert(std::make_pair<std::string, std::vector<data_t>>(move(keys[i]), std::forward<std::vector<data_t>>(values[i])));
			}
		}
		return result;
	}

public:

	static std::map<std::string, std::vector<data_t>> read(const std::filesystem::path& fromFile) {
		using namespace std;
		ifstream fileReader(fromFile);
		vector<string> mapKeys(getKeys(fileReader));
		vector<vector<data_t>>mapValues(getValues(fileReader, mapKeys.size()));

		return linkKeysAndValues(mapKeys, mapValues);
	}
};

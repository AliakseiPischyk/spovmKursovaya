#pragma once
#include <algorithm>
#include <vector>
#include <functional>
#include <optional>
#include <memory>

#include "DataSizeNotEqualException.h"

template<class data_t, class = std::enable_if_t<std::is_pointer_v<data_t>>>
class Matrix {

private:
	std::unique_ptr<data_t> begin;
	size_t rows;
	size_t columns;

	static bool correctSizesPassed(const std::vector<std::remove_pointer<data_t>>& dependentData,
		const std::vector<std::vector<std::remove_pointer<data_t>>>& independentData){

		const size_t dataSize = dependentData.size();
		for (const auto& column : independentData) {
			if (column.size() != dataSize) {
				return false;
			}
		}
		return true;
	}

public:

	Matrix() {};

	Matrix(const std::vector<std::remove_pointer<data_t>>& dependentData,
		const std::vector<std::vector<std::remove_pointer<data_t>>>& independentData ) 
		noexcept(std::false_type){
		
		if (!correctSizesPassed(dependentData, independentData)) {
			throw DataSizeNotEqualException;
		}
		////////////////////////////
	}

	static void multiply(const Matrix<data_t>& source1, 
		const Matrix<data_t>& source2, 
		Matrix<data_t>& dest)
	noexcept(std::false_type){

	}

	static void transpose(const Matrix<data_t>& source,
		Matrix<data_t>& dest) {

	}

	static void inverse(const Matrix<data_t>& source,
		Matrix<data_t>& dest) {

	}

	~Matrix() {
		free(begin.get());
	}
};

template<class data_t, class = std::enable_if_t<std::is_floating_point_v<data_t>>>
class MultipleRegressionCalculator
{
public:
	using data_t_ptr = std::add_pointer_t<data_t>;

	static bool calc(const Matrix<data_t_ptr>& dependentData,
		const Matrix<data_t_ptr>& independentData) {

		Matrix<data_t_ptr> transIndep;
		Matrix<data_t_ptr>::transpose(independentData, transIndep);

		Matrix<data_t_ptr> multTransIndepByIndep;
		Matrix<data_t_ptr>::multiply(transIndep, independentData, multTransIndepByIndep);

		Matrix<data_t_ptr> multTransIndepByDep;
		Matrix<data_t_ptr>::multiply(transIndep, dependentData, multTransIndepByDep);

		Matrix<data_t_ptr> inserveMultTransIndepByIndep;
		Matrix<data_t_ptr>::inverse(multTransIndepByIndep, inserveMultTransIndepByIndep);

		Matrix<data_t_ptr> equationCoefficients;
		Matrix<data_t_ptr>::multiply(inserveMultTransIndepByIndep, multTransIndepByDep, equationCoefficients);
	}
};


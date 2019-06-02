#pragma once
#include <algorithm>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>

#include "DataSizeNotEqualException.h"

size_t roundDown(size_t value, int doKuda) {
	return value - (value % doKuda);
}

template<class data_t, class = std::enable_if_t<std::is_pointer_v<data_t>>>
class Matrix {

private:
	std::unique_ptr<data_t> begin;
	size_t rows;
	size_t columns;

	static bool correctSizesPassed(const std::vector<std::remove_pointer<data_t>>& dependentData,
		const std::vector<std::vector<std::remove_pointer<data_t>>>& independentData) {

		const size_t dataSize = dependentData.size();
		for (const auto& column : independentData) {
			if (column.size() != dataSize) {
				return false;
			}
		}
		return true;
	}

	struct buf_t
	{
		float * p;
		int n;

		buf_t(int size) : n(size), p((float*)_mm_malloc(size * 4, 64)) {}
		~buf_t() { _mm_free(p); }
	};

	static void init_c(int M, int N, float * C, int ldc)
	{
		for (int i = 0; i < M; ++i, C += ldc)
			for (int j = 0; j < N; j += 8)
				_mm256_storeu_ps(C + j, _mm256_setzero_ps());
	}

	static void reorder_b_16(int K, const float * B, int ldb, float * bufB)
	{
		for (int k = 0; k < K; ++k, B += ldb, bufB += 16)
		{
			_mm256_storeu_ps(bufB + 0, _mm256_loadu_ps(B + 0));
			_mm256_storeu_ps(bufB + 8, _mm256_loadu_ps(B + 8));
		}
	}

	static void reorder_a_6(const float * A, int lda, int M, int K, float * bufA)
	{
		for (int i = 0; i < M; i += 6)
		{
			for (int k = 0; k < K; k += 4)
			{
				const float * pA = A + k;
				__m128 a0 = _mm_loadu_ps(pA + 0 * lda);
				__m128 a1 = _mm_loadu_ps(pA + 1 * lda);
				__m128 a2 = _mm_loadu_ps(pA + 2 * lda);
				__m128 a3 = _mm_loadu_ps(pA + 3 * lda);
				__m128 a4 = _mm_loadu_ps(pA + 4 * lda);
				__m128 a5 = _mm_loadu_ps(pA + 5 * lda);
				__m128 a00 = _mm_unpacklo_ps(a0, a2);
				__m128 a01 = _mm_unpacklo_ps(a1, a3);
				__m128 a10 = _mm_unpackhi_ps(a0, a2);
				__m128 a11 = _mm_unpackhi_ps(a1, a3);
				__m128 a20 = _mm_unpacklo_ps(a4, a5);
				__m128 a21 = _mm_unpackhi_ps(a4, a5);
				_mm_storeu_ps(bufA + 0, _mm_unpacklo_ps(a00, a01));
				_mm_storel_pi((__m64*)(bufA + 4), a20);
				_mm_storeu_ps(bufA + 6, _mm_unpackhi_ps(a00, a01));
				_mm_storeh_pi((__m64*)(bufA + 10), a20);
				_mm_storeu_ps(bufA + 12, _mm_unpacklo_ps(a10, a11));
				_mm_storel_pi((__m64*)(bufA + 16), a21);
				_mm_storeu_ps(bufA + 18, _mm_unpackhi_ps(a10, a11));
				_mm_storeh_pi((__m64*)(bufA + 22), a21);
				bufA += 24;
			}
			A += 6 * lda;
		}
	}

	static void micro_6x16(int K, const float * A, int lda, int step,
		const float * B, int ldb, float * C, int ldc)
	{
		__m256 c00 = _mm256_setzero_ps();
		__m256 c10 = _mm256_setzero_ps();
		__m256 c20 = _mm256_setzero_ps();
		__m256 c30 = _mm256_setzero_ps();
		__m256 c40 = _mm256_setzero_ps();
		__m256 c50 = _mm256_setzero_ps();
		__m256 c01 = _mm256_setzero_ps();
		__m256 c11 = _mm256_setzero_ps();
		__m256 c21 = _mm256_setzero_ps();
		__m256 c31 = _mm256_setzero_ps();
		__m256 c41 = _mm256_setzero_ps();
		__m256 c51 = _mm256_setzero_ps();
		const int offset0 = lda * 0;
		const int offset1 = lda * 1;
		const int offset2 = lda * 2;
		const int offset3 = lda * 3;
		const int offset4 = lda * 4;
		const int offset5 = lda * 5;
		__m256 b0, b1, a0, a1;
		for (int k = 0; k < K; k++)
		{
			b0 = _mm256_loadu_ps(B + 0);
			b1 = _mm256_loadu_ps(B + 8);
			a0 = _mm256_set1_ps(A[offset0]);
			a1 = _mm256_set1_ps(A[offset1]);
			c00 = _mm256_fmadd_ps(a0, b0, c00);
			c01 = _mm256_fmadd_ps(a0, b1, c01);
			c10 = _mm256_fmadd_ps(a1, b0, c10);
			c11 = _mm256_fmadd_ps(a1, b1, c11);
			a0 = _mm256_set1_ps(A[offset2]);
			a1 = _mm256_set1_ps(A[offset3]);
			c20 = _mm256_fmadd_ps(a0, b0, c20);
			c21 = _mm256_fmadd_ps(a0, b1, c21);
			c30 = _mm256_fmadd_ps(a1, b0, c30);
			c31 = _mm256_fmadd_ps(a1, b1, c31);
			a0 = _mm256_set1_ps(A[offset4]);
			a1 = _mm256_set1_ps(A[offset5]);
			c40 = _mm256_fmadd_ps(a0, b0, c40);
			c41 = _mm256_fmadd_ps(a0, b1, c41);
			c50 = _mm256_fmadd_ps(a1, b0, c50);
			c51 = _mm256_fmadd_ps(a1, b1, c51);
			B += ldb; A += step;
		}
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c00, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c01, _mm256_loadu_ps(C + 8)));
		C += ldc;
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c10, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c11, _mm256_loadu_ps(C + 8)));
		C += ldc;
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c20, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c21, _mm256_loadu_ps(C + 8)));
		C += ldc;
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c30, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c31, _mm256_loadu_ps(C + 8)));
		C += ldc;
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c40, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c41, _mm256_loadu_ps(C + 8)));
		C += ldc;
		_mm256_storeu_ps(C + 0, _mm256_add_ps(c50, _mm256_loadu_ps(C + 0)));
		_mm256_storeu_ps(C + 8, _mm256_add_ps(c51, _mm256_loadu_ps(C + 8)));
	}

	static void macro_v7(int M, int N, int K, const float * A,
		const float * B, int ldb, float * bufB, bool reorderB, float * C, int ldc)
	{
		for (int j = 0; j < N; j += 16)
		{
			if (reorderB)
				reorder_b_16(K, B + j, ldb, bufB + K * j);
			for (int i = 0; i < M; i += 6)
				micro_6x16(K, A + i * K, 1, 6, bufB + K * j, 16, C + i * ldc + j, ldc);
		}
	}

public:
	static void fastMult(const size_t M, const size_t N, const size_t K, const float const* A, const float const* B, float * C)
	{
		using cacheBytes = size_t;
		constexpr cacheBytes L1 = 32 * 1024;
		constexpr cacheBytes L2 = 1024 * 1024;
		constexpr cacheBytes L3 = 8 * 1024 * 1024;
		size_t minK = std::min(L1 / 4 / 16, K) / 4 * 4;
		size_t minM = std::min(L2 / 4 / minK, M) / 6 * 6;
		size_t minN = std::min(L3 / 4 / minK, N) / 16 * 16;
		buf_t bufB(minN * minK);
		buf_t bufA(minK * minM);
		for (size_t j = 0; j < N; j += minN)
		{
			int dN = std::min(N, j + minN) - j;
			for (int k = 0; k < K; k += minK)
			{
				int dK = std::min(K, k + minK) - k;
				for (int i = 0; i < M; i += minM)
				{
					int dM = std::min(M, i + minM) - i;
					if (k == 0)
						init_c(dM, dN, C + i * N + j, N);
					reorder_a_6(A + i * K + k, K, dM, dK, bufA.p);
					macro_v7(dM, dN, dK, bufA.p, B + k * N + j, N,
						bufB.p, i == 0, C + i * N + j, N);
				}
			}
		}
	}

public:

	Matrix() {};

	Matrix(const std::vector<std::remove_pointer<data_t>>& dependentData,
		const std::vector<std::vector<std::remove_pointer<data_t>>>& independentData)
		noexcept(std::false_type) {

		if (!correctSizesPassed(dependentData, independentData)) {
			throw DataSizeNotEqualException;
		}
		////////////////////////////
	}

	static void multiply(
		const Matrix<data_t>& source1,
		const Matrix<data_t>& source2,
		Matrix<data_t>& dest)
		noexcept(std::false_type) {

		fastMult(source1.rows, source2.rows, source2.columns,
			source1.begin.get(), source2.begin.get(), dest.begin.get());

	}

	__forceinline bool wasNotAllocated()const {
		return this.begin == nullptr;
	}

	static __forceinline void allocate(Matrix<data_t>& matrix,
		const size_t rows,
		const size_t columns) {

		matrix.rows = rows;
		matrix.columns = columns;
		matrix.begin = new std::remove_pointer<data_t>[rows*columns];

		if (Matrix::wasNotAllocated(matrix)) {
			exit(0);
		}
	}

	static __forceinline void transpose(
		const float const* source,
		float* dest,
		const size_t rows,
		const size_t columns) {

#pragma omp parallel for
		for (size_t n = 0; n < columns*rows; n++) {
			const size_t k = n / rows;
			const size_t i = n % rows;
			dest[n] = source[columns*i + k + n];
		}
	}

	static void transpose(const Matrix<data_t>& source,
		Matrix<data_t>& dest) {

		dest.rows = source.columns;
		dest.columns = source.rows;

		if (dest.wasNotAllocated()) {
			Matrix::allocate(dest, dest.rows, dest.columns);
		}

		if (source.rows == 1 || source.columns == 1) {

			memcpy(dest.begin, source.begin, source.rows * source.columns);
		}
		else {
			transpose(source.begin, dest.begin, source.rows, source.columns);
		}

	}

	static void inverse(const Matrix<data_t>& source,
		Matrix<data_t>& dest) {

	}

	~Matrix() {
		free(begin.get());
	}
};

template<class data_t, class = is_floating<data_t>>
class MultipleRegressionCalculator
{
public:
	using data_t_ptr = std::add_pointer_t<data_t>;

	static auto calculate(const Matrix<data_t_ptr>& dependentData,
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

		return equationCoefficients;
	}
};


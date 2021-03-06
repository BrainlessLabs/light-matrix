/**
 * @file test_ewise_accum.cpp
 *
 * @brief Unit testing of element-wise accumulation
 *
 * @author Dahua Lin
 */

#include "../test_base.h"

#define DEFAULT_M_VALUE 13
#define DEFAULT_N_VALUE 9

#include "../multimat_supp.h"

#include <light_mat/matrix/matrix_classes.h>
#include <light_mat/mateval/ewise_eval.h>
#include <light_mat/math/functor_base.h>

using namespace lmat;
using namespace lmat::test;

template<typename T>
struct my_sum_kernel
{
	typedef T value_type;

	LMAT_ENSURE_INLINE
	void operator() (const T& x, T& s) const
	{
		s += x;
	}
};

template<typename T>
struct my_max_kernel
{
	typedef T value_type;

	LMAT_ENSURE_INLINE
	void operator() (const T& x, T& s) const
	{
		s = math::max(s, x);
	}
};


template<typename T>
struct my_min_kernel
{
	typedef T value_type;

	LMAT_ENSURE_INLINE
	void operator() (const T& x, T& s) const
	{
		s = math::min(s, x);
	}
};


namespace lmat {

	LMAT_DECL_SIMDIZABLE_ON_REAL( my_sum_kernel )
	LMAT_DEF_TRIVIAL_SIMDIZE_MAP( my_sum_kernel )

	LMAT_DECL_SIMDIZABLE_ON_REAL( my_min_kernel )
	LMAT_DEF_TRIVIAL_SIMDIZE_MAP( my_min_kernel )

	LMAT_DECL_SIMDIZABLE_ON_REAL( my_max_kernel )
	LMAT_DEF_TRIVIAL_SIMDIZE_MAP( my_max_kernel )
}



template<typename U, int M, int N>
void test_linear_accum()
{
	const index_t m = M == 0 ? DM : M;
	const index_t n = N == 0 ? DN : N;

	typedef typename mat_host<cont, double, M, N>::cmat_t smat_t;

	mat_host<cont, double, M, N> src(m, n);
	src.fill_rand();

	smat_t smat = src.get_cmat();
	matrix_shape<M, N> shape(m, n);

	// sum

	double v_sum = 10.0;
	for (index_t i = 0; i < m * n; ++i) v_sum += smat[i];

	double r_sum = 10.0;
	ewise(my_sum_kernel<double>()).eval(macc_<linear_, U>(), shape, in_(smat), sum_to_(r_sum));

	ASSERT_APPROX(r_sum, v_sum, 1.0e-12);

	// max

	double v_max = 0.5;
	for (index_t i = 0; i < m * n; ++i) if (v_max < smat[i]) v_max = smat[i];

	double r_max = 0.5;
	ewise(my_max_kernel<double>()).eval(macc_<linear_, U>(), shape, in_(smat), max_to_(r_max));

	ASSERT_EQ( v_max, r_max );

	// min

	double v_min = 0.5;
	for (index_t i = 0; i < m * n; ++i) if (v_min > smat[i]) v_min = smat[i];

	double r_min = 0.5;
	ewise(my_min_kernel<double>()).eval(macc_<linear_, U>(), shape, in_(smat), min_to_(r_min));

	ASSERT_EQ( v_min, r_min );
}


template<typename U, int M, int N>
void test_percol_accum()
{
	const index_t m = M == 0 ? DM : M;
	const index_t n = N == 0 ? DN : N;

	typedef typename mat_host<cont, double, M, N>::cmat_t smat_t;

	mat_host<cont, double, M, N> src(m, n);
	src.fill_rand();

	smat_t smat = src.get_cmat();
	matrix_shape<M, N> shape(m, n);

	// sum

	double v_sum = 10.0;
	for (index_t i = 0; i < m * n; ++i) v_sum += smat[i];

	double r_sum = 10.0;
	ewise(my_sum_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), sum_to_(r_sum));

	ASSERT_APPROX(r_sum, v_sum, 1.0e-12);

	// max

	double v_max = 0.5;
	for (index_t i = 0; i < m * n; ++i) if (v_max < smat[i]) v_max = smat[i];

	double r_max = 0.5;
	ewise(my_max_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), max_to_(r_max));

	ASSERT_EQ( v_max, r_max );

	// min

	double v_min = 0.5;
	for (index_t i = 0; i < m * n; ++i) if (v_min > smat[i]) v_min = smat[i];

	double r_min = 0.5;
	ewise(my_min_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), min_to_(r_min));

	ASSERT_EQ( v_min, r_min );
}


template<typename U, typename DTag, int M, int N>
void test_accum_colwise()
{
	const index_t m = M == 0 ? DM : M;
	const index_t n = N == 0 ? DN : N;

	typedef typename mat_host<cont, double, M, N>::cmat_t smat_t;
	typedef typename mat_host<DTag, double, 1, N>::mat_t dmat_t;
	typedef dense_row<double, N> rrow_t;

	mat_host<cont, double, M, N> src(m, n);
	src.fill_rand();
	mat_host<DTag, double, 1, N> dst(1, n);

	smat_t smat = src.get_cmat();
	dmat_t drow = dst.get_mat();
	rrow_t rrow(n);

	matrix_shape<M, N> shape(m, n);

	// sum

	for (index_t j = 0; j < n; ++j)
	{
		rrow[j] = double(2 * j + 1);
		drow[j] = rrow[j];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			rrow[j] += smat(i, j);
		}
	}

	ewise(my_sum_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), colwise_sum_to_(drow));

	ASSERT_MAT_APPROX( 1, n, drow, rrow, 1.0e-12 );

	// max

	for (index_t j = 0; j < n; ++j)
	{
		rrow[j] = 0.5;
		drow[j] = rrow[j];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			if (rrow[j] < smat(i, j))
				rrow[j] = smat(i, j);
		}
	}

	ewise(my_max_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), colwise_max_to_(drow));

	ASSERT_MAT_EQ( 1, n, drow, rrow );


	// min

	for (index_t j = 0; j < n; ++j)
	{
		rrow[j] = 0.5;
		drow[j] = rrow[j];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			if (rrow[j] > smat(i, j))
				rrow[j] = smat(i, j);
		}
	}

	ewise(my_min_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), colwise_min_to_(drow));

	ASSERT_MAT_EQ( 1, n, drow, rrow );
}


template<typename U, typename DTag, int M, int N>
void test_accum_rowwise()
{
	const index_t m = M == 0 ? DM : M;
	const index_t n = N == 0 ? DN : N;

	typedef typename mat_host<cont, double, M, N>::cmat_t smat_t;
	typedef typename mat_host<DTag, double, M, 1>::mat_t dmat_t;
	typedef dense_col<double, M> rcol_t;

	mat_host<cont, double, M, N> src(m, n);
	src.fill_rand();
	mat_host<DTag, double, M, 1> dst(m, 1);

	smat_t smat = src.get_cmat();
	dmat_t dcol = dst.get_mat();
	rcol_t rcol(m);

	matrix_shape<M, N> shape(m, n);

	// sum

	for (index_t i = 0; i < m; ++i)
	{
		rcol[i] = double(2 * i + 1);
		dcol[i] = rcol[i];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			rcol[i] += smat(i, j);
		}
	}

	ewise(my_sum_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), rowwise_sum_to_(dcol));

	ASSERT_MAT_APPROX( m, 1, dcol, rcol, 1.0e-12 );

	// max

	for (index_t i = 0; i < m; ++i)
	{
		rcol[i] = 0.5;
		dcol[i] = rcol[i];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			if (rcol[i] < smat(i, j))
				rcol[i] = smat(i, j);
		}
	}

	ewise(my_max_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), rowwise_max_to_(dcol));

	ASSERT_MAT_EQ( m, 1, dcol, rcol );

	// min

	for (index_t i = 0; i < m; ++i)
	{
		rcol[i] = 0.5;
		dcol[i] = rcol[i];
	}

	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
		{
			if (rcol[i] > smat(i, j))
				rcol[i] = smat(i, j);
		}
	}

	ewise(my_min_kernel<double>()).eval(macc_<percol_, U>(), shape,
			in_(smat), rowwise_min_to_(dcol));

	ASSERT_MAT_EQ( m, 1, dcol, rcol );
}



// accum linear

MN_CASE( ewise_accum_linear_scalar )
{
	test_linear_accum<scalar_, M, N>();
}

MN_CASE( ewise_accum_linear_sse )
{
	test_linear_accum<simd_<sse_t>, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_linear_avx )
{
	test_linear_accum<simd_<avx_t>, M, N>();
}

#endif


// accum percol

MN_CASE( ewise_accum_percol_scalar )
{
	test_percol_accum<scalar_, M, N>();
}

MN_CASE( ewise_accum_percol_sse )
{
	test_percol_accum<simd_<sse_t>, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_percol_avx )
{
	test_percol_accum<simd_<avx_t>, M, N>();
}

#endif


// accum colwise

MN_CASE( ewise_accum_colwise_scalar_cont )
{
	test_accum_colwise<scalar_, cont, M, N>();
}

MN_CASE( ewise_accum_colwise_sse_cont )
{
	test_accum_colwise<simd_<sse_t>, cont, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_colwise_avx_cont )
{
	test_accum_colwise<simd_<avx_t>, cont, M, N>();
}

#endif

MN_CASE( ewise_accum_colwise_scalar_bloc )
{
	test_accum_colwise<scalar_, bloc, M, N>();
}

MN_CASE( ewise_accum_colwise_sse_bloc )
{
	test_accum_colwise<simd_<sse_t>, bloc, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_colwise_avx_bloc )
{
	test_accum_colwise<simd_<avx_t>, bloc, M, N>();
}

#endif

MN_CASE( ewise_accum_colwise_scalar_grid )
{
	test_accum_colwise<scalar_, grid, M, N>();
}

MN_CASE( ewise_accum_colwise_sse_grid )
{
	test_accum_colwise<simd_<sse_t>, grid, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_colwise_avx_grid )
{
	test_accum_colwise<simd_<avx_t>, grid, M, N>();
}

#endif


// accum rowwise

MN_CASE( ewise_accum_rowwise_scalar_cont )
{
	test_accum_rowwise<scalar_, cont, M, N>();
}

MN_CASE( ewise_accum_rowwise_sse_cont )
{
	test_accum_rowwise<simd_<sse_t>, cont, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_rowwise_avx_cont )
{
	test_accum_rowwise<simd_<avx_t>, cont, M, N>();
}

#endif

MN_CASE( ewise_accum_rowwise_scalar_bloc )
{
	test_accum_rowwise<scalar_, bloc, M, N>();
}

MN_CASE( ewise_accum_rowwise_sse_bloc )
{
	test_accum_rowwise<simd_<sse_t>, bloc, M, N>();
}

#ifdef LMAT_HAS_AVX

MN_CASE( ewise_accum_rowwise_avx_bloc )
{
	test_accum_rowwise<simd_<avx_t>, bloc, M, N>();
}

#endif

MN_CASE( ewise_accum_rowwise_scalar_grid )
{
	test_accum_rowwise<scalar_, grid, M, N>();
}



// Test Packs


AUTO_TPACK( accum_linear_scalar )
{
	ADD_MN_CASE_3X3( ewise_accum_linear_scalar, DM, DN )
}

AUTO_TPACK( accum_linear_sse )
{
	ADD_MN_CASE_3X3( ewise_accum_linear_sse, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_linear_avx )
{
	ADD_MN_CASE_3X3( ewise_accum_linear_avx, DM, DN )
}

#endif

AUTO_TPACK( accum_percol_scalar )
{
	ADD_MN_CASE_3X3( ewise_accum_percol_scalar, DM, DN )
}

AUTO_TPACK( accum_percol_sse )
{
	ADD_MN_CASE_3X3( ewise_accum_percol_sse, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_percol_avx )
{
	ADD_MN_CASE_3X3( ewise_accum_percol_avx, DM, DN )
}

#endif

AUTO_TPACK( accum_colwise_scalar_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_scalar_cont, DM, DN )
}

AUTO_TPACK( accum_colwise_sse_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_sse_cont, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_colwise_avx_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_avx_cont, DM, DN )
}

#endif

AUTO_TPACK( accum_colwise_scalar_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_scalar_bloc, DM, DN )
}

AUTO_TPACK( accum_colwise_sse_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_sse_bloc, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_colwise_avx_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_avx_bloc, DM, DN )
}

#endif

AUTO_TPACK( accum_colwise_scalar_grid )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_scalar_grid, DM, DN )
}

AUTO_TPACK( accum_colwise_sse_grid )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_sse_grid, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_colwise_avx_grid )
{
	ADD_MN_CASE_3X3( ewise_accum_colwise_avx_grid, DM, DN )
}

#endif


AUTO_TPACK( accum_rowwise_scalar_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_scalar_cont, DM, DN )
}

AUTO_TPACK( accum_rowwise_sse_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_sse_cont, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_rowwise_avx_cont )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_avx_cont, DM, DN )
}

#endif

AUTO_TPACK( accum_rowwise_scalar_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_scalar_bloc, DM, DN )
}

AUTO_TPACK( accum_rowwise_sse_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_sse_bloc, DM, DN )
}

#ifdef LMAT_HAS_AVX

AUTO_TPACK( accum_rowwise_avx_bloc )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_avx_bloc, DM, DN )
}

#endif

AUTO_TPACK( accum_rowwise_scalar_grid )
{
	ADD_MN_CASE_3X3( ewise_accum_rowwise_scalar_grid, DM, DN )
}

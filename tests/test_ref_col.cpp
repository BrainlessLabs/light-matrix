/**
 * @file test_ref_col.cpp
 *
 * Unit testing of cref_col and ref_col class.
 *
 * @author Dahua Lin
 */


#include "test_base.h"

#include <light_mat/matrix/ref_matrix.h>
#include <light_mat/core/array.h>

using namespace lmat;
using namespace lmat::test;

// explicit instantiation

template class lmat::ref_col<double, DynamicDim>;
template class lmat::ref_col<double, 4>;

#ifdef LMAT_USE_STATIC_ASSERT

static_assert(lmat::is_base_of<
		lmat::ref_matrix<double, DynamicDim, 1>,
		lmat::ref_col<double, DynamicDim> >::value, "Base verification failed.");

static_assert(lmat::is_base_of<
		lmat::ref_matrix<double, 4, 1>,
		lmat::ref_col<double, 4> >::value, "Base verification failed.");

#endif

N_CASE( cref_col, constructs )
{
	const index_t n = N == 0 ? 4 : N;

	scoped_array<double> s(n);
	const double *ps = s.ptr_begin();

	cref_col<double, N> a(ps, n);

	ASSERT_EQ(a.nrows(), n);
	ASSERT_EQ(a.ncolumns(), 1);
	ASSERT_EQ(a.nelems(), n);
	ASSERT_EQ(a.lead_dim(), n);
	ASSERT_EQ(a.size(), (size_t)a.nelems() );

	ASSERT_EQ(a.ptr_data(), ps);

	cref_col<double, N> a2(a);

	ASSERT_EQ(a2.nrows(), n);
	ASSERT_EQ(a2.ncolumns(), 1);
	ASSERT_EQ(a2.nelems(), n);
	ASSERT_EQ(a2.lead_dim(), n);
	ASSERT_EQ(a2.size(), (size_t)a2.nelems() );

	ASSERT_EQ(a.ptr_data(), ps);
}


N_CASE( ref_col, constructs )
{
	const index_t n = N == 0 ? 4 : N;

	scoped_array<double> s(n);
	double *ps = s.ptr_begin();

	ref_col<double, N> a(ps, n);

	ASSERT_EQ(a.nrows(), n);
	ASSERT_EQ(a.ncolumns(), 1);
	ASSERT_EQ(a.nelems(), n);
	ASSERT_EQ(a.lead_dim(), n);
	ASSERT_EQ(a.size(), (size_t)a.nelems() );

	ASSERT_EQ(a.ptr_data(), ps);

	ref_col<double, N> a2(a);

	ASSERT_EQ(a2.nrows(), n);
	ASSERT_EQ(a2.ncolumns(), 1);
	ASSERT_EQ(a2.nelems(), n);
	ASSERT_EQ(a2.lead_dim(), n);
	ASSERT_EQ(a2.size(), (size_t)a2.nelems() );

	ASSERT_EQ(a.ptr_data(), ps);
}


N_CASE( ref_col, assign )
{
	const index_t n = N == 0 ? 4 : N;

	scoped_array<double> s1(n);
	scoped_array<double> s2(n);

	double *ps1 = s1.ptr_begin();
	double *ps2 = s2.ptr_begin();

	for (index_t i = 0; i < n; ++i) s1[i] = double(i + 2);
	for (index_t i = 0; i < n; ++i) s2[i] = double(2 * i + 3);

	ref_col<double, N> a1(ps1, n);
	ref_col<double, N> a2(ps2, n);

	ASSERT_EQ( a1.ptr_data(), ps1 );
	ASSERT_EQ( a2.ptr_data(), ps2 );
	ASSERT_NE( ps1, ps2 );

	a1 = a2;

	ASSERT_EQ( a1.ptr_data(), ps1 );
	ASSERT_EQ( a2.ptr_data(), ps2 );

	ASSERT_VEC_EQ( n, a1, a2 );
}


N_CASE( ref_col, assign_gen )
{
	const index_t n = N == 0 ? 4 : N;

	scoped_array<double> ref(n);
	scoped_array<double> s(n);
	fill(s, -1.0);

	// zeros

	double *ps = s.ptr_begin();
	ref_col<double, N> a(ps, n);

	a = zeros<double>();
	for (index_t i = 0; i < n; ++i) ref[i] = double(0);

	ASSERT_EQ(a.ptr_data(), ps);
	ASSERT_VEC_EQ(n, a, ref);

	// fill_value

	const double v1 = 2.5;
	a = fill_value(v1);
	for (index_t i = 0; i < n; ++i) ref[i] = v1;

	ASSERT_EQ(a.ptr_data(), ps);
	ASSERT_VEC_EQ(n, a, ref);

	// copy_value

	for (index_t i = 0; i < n; ++i) ref[i] = double(i + 2);
	a = copy_from(ref.ptr_begin());

	ASSERT_EQ(a.ptr_data(), ps);
	ASSERT_VEC_EQ(n, a, ref);
}


BEGIN_TPACK( cref_col_constructs )
	ADD_N_CASE( cref_col, constructs, 0 )
	ADD_N_CASE( cref_col, constructs, 1 )
	ADD_N_CASE( cref_col, constructs, 4 )
END_TPACK

BEGIN_TPACK( ref_col_constructs )
	ADD_N_CASE( ref_col, constructs, 0 )
	ADD_N_CASE( ref_col, constructs, 1 )
	ADD_N_CASE( ref_col, constructs, 4 )
END_TPACK

BEGIN_TPACK( ref_col_assign )
	ADD_N_CASE( ref_col, assign, 0 )
	ADD_N_CASE( ref_col, assign, 1 )
	ADD_N_CASE( ref_col, assign, 4 )
END_TPACK

BEGIN_TPACK( ref_col_assign_gen )
	ADD_N_CASE( ref_col, assign_gen, 0 )
	ADD_N_CASE( ref_col, assign_gen, 1 )
	ADD_N_CASE( ref_col, assign_gen, 4 )
END_TPACK


BEGIN_MAIN_SUITE
	ADD_TPACK( cref_col_constructs )

	ADD_TPACK( ref_col_constructs )
	ADD_TPACK( ref_col_assign )
	ADD_TPACK( ref_col_assign_gen )
END_MAIN_SUITE




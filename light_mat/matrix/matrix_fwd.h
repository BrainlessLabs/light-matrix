/**
 * @file matrix_fwd.h
 *
 * Forward declaration of Matrix-related classes
 *
 * @author Dahua Lin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef LIGHTMAT_MATRIX_FWD_H_
#define LIGHTMAT_MATRIX_FWD_H_

#include <light_mat/common/basic_defs.h>
#include <light_mat/common/range.h>
#include <light_mat/common/memory.h>

#include <light_mat/matrix/matrix_shape.h>

namespace lmat
{
	// forward declaration of concepts


	/****************************************************************
	 *
	 *   A specialized version of matrix_traits<C> must be provided,
	 *   which should contain the following static members:
	 *
	 *   - num_dimensions:	an int value, which must be set to 2
	 *   					(reserved for future extension)
	 *
	 *   - ct_num_rows:		compile-time number of rows
	 *   - ct_num_cols:		compile-time number of columns
	 *   - is_readonly:		whether the contents can be modified
	 *
	 *	 - value_type:	    the type of element value
	 *	 - domain:			the domain (e.g. cpu_domain, cuda_domain)
	 *
	 ****************************************************************/

	struct cpu_domain { };
	struct cuda_domain { };

	template<class Layout> struct layout_traits;

	template<class Derived>
	struct matrix_traits
	{
		typedef nil_t value_type;
	};

	template<typename T, index_t M, index_t N, typename Domain>
	struct matrix_xpr_traits_base
	{
		static const unsigned int num_dimensions = 2;

		static const index_t ct_num_rows = M;
		static const index_t ct_num_cols = N;

		typedef typename std::remove_cv<T>::type value_type;

		typedef Domain domain;
	};

	template<typename QT, index_t M, index_t N, typename Domain>
	struct regular_matrix_traits_base
	{
		static const unsigned int num_dimensions = 2;

		static const index_t ct_num_rows = M;
		static const index_t ct_num_cols = N;

		typedef QT qualified_value_type;
		typedef typename std::remove_cv<QT>::type value_type;

		typedef Domain domain;
	};


	template<class Derived, typename T> class IMatrixXpr;
	template<class Derived, typename T> class IEWiseMatrix;
	template<class Derived, typename T> class IRegularMatrix;

	// forward declaration of some important types

	template<typename T, index_t CM=0, index_t CN=0> class dense_matrix;
	template<typename T, index_t CM=0> class dense_col;
	template<typename T, index_t CN=0> class dense_row;

	template<typename T, index_t CM=0, index_t CN=0> class cref_matrix;
	template<typename T, index_t CM=0, index_t CN=0> class ref_matrix;
	template<typename T, index_t CM=0> class cref_col;
	template<typename T, index_t CN=0> class cref_row;
	template<typename T, index_t CM=0> class ref_col;
	template<typename T, index_t CN=0> class ref_row;

	template<typename T, index_t CM=0, index_t CN=0> class cref_block;
	template<typename T, index_t CM=0, index_t CN=0> class ref_block;
	template<typename T, index_t CM=0, index_t CN=0> class cref_grid;
	template<typename T, index_t CM=0, index_t CN=0> class ref_grid;

	template<typename T, index_t CM=0> class cstep_col;
	template<typename T, index_t CM=0> class step_col;
	template<typename T, index_t CN=0> class cstep_row;
	template<typename T, index_t CN=0> class step_row;

	template<class Mat> class dense_mutable_view;

	template<typename T, index_t CM, index_t CN> class inds_expr;
	template<typename T, index_t CM, index_t CN> class subs_i_expr;
	template<typename T, index_t CM, index_t CN> class subs_j_expr;

	// contiguous levels

	namespace cont_level
	{
		struct whole { };
		struct percol { };
		struct none { };
	}

	// iterations

	template<class Mat> struct matrix_iter;

	// subviews

	template<class Mat, typename Range> struct vecview_map;
	template<class Mat, typename RowRange> struct colview_map;
	template<class Mat, typename ColRange> struct rowview_map;
	template<class Mat> struct diagview_map;
	template<class Mat, typename RowRange, typename ColRange> struct matview_map;

}

// Useful macros

#define LMAT_MATRIX_TYPEDEFS0(TName, prefix) \
	typedef TName<double>   prefix##_f64; \
	typedef TName<float>    prefix##_f32; \
	typedef TName<int32_t>  prefix##_i32; \
	typedef TName<uint32_t> prefix##_u32; \
	typedef TName<int16_t>  prefix##_i16; \
	typedef TName<uint16_t> prefix##_u16; \
	typedef TName<int8_t>   prefix##_i8; \
	typedef TName<uint8_t>  prefix##_u8; \
	typedef TName<bool>     prefix##_bool;

#define LMAT_MATRIX_TYPEDEFS1(TName, prefix, Dim) \
	typedef TName<double,   Dim> prefix##_f64; \
	typedef TName<float,    Dim> prefix##_f32; \
	typedef TName<int32_t,  Dim> prefix##_i32; \
	typedef TName<uint32_t, Dim> prefix##_u32; \
	typedef TName<int16_t,  Dim> prefix##_i16; \
	typedef TName<uint16_t, Dim> prefix##_u16; \
	typedef TName<int8_t,   Dim> prefix##_i8; \
	typedef TName<uint8_t,  Dim> prefix##_u8; \
	typedef TName<bool,     Dim> prefix##_bool;

#define LMAT_MATRIX_TYPEDEFS2(TName, prefix, RDim, CDim) \
	typedef TName<double,   RDim, CDim> prefix##_f64; \
	typedef TName<float,    RDim, CDim> prefix##_f32; \
	typedef TName<int32_t,  RDim, CDim> prefix##_i32; \
	typedef TName<uint32_t, RDim, CDim> prefix##_u32; \
	typedef TName<int16_t,  RDim, CDim> prefix##_i16; \
	typedef TName<uint16_t, RDim, CDim> prefix##_u16; \
	typedef TName<int8_t,   RDim, CDim> prefix##_i8; \
	typedef TName<uint8_t,  RDim, CDim> prefix##_u8; \
	typedef TName<bool,     RDim, CDim> prefix##_bool;


#endif



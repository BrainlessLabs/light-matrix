/**
 * @file int_div.h
 *
 * @brief Facilities for integer division
 *
 * @author Dahua Lin
 */

#ifndef LIGHTMAT_INT_DIV_H_
#define LIGHTMAT_INT_DIV_H_

#include <light_mat/common/prim_types.h>


#define LMAT_DEFINE_INT_DIV_C( L, D ) \
	template<> struct int_div<D> { \
		LMAT_ENSURE_INLINE \
		static index_t quo(index_t n) { return n >> L; } \
		LMAT_ENSURE_INLINE \
		static index_t rem(index_t n) { return n & (D-1); } \
		LMAT_ENSURE_INLINE \
		static index_t maj(index_t n) { return (n >> L) << L; } \
	};


namespace lmat
{

	template<unsigned int D> struct int_div;

	template<>
	struct int_div<1>
	{
		LMAT_ENSURE_INLINE
		static index_t quo(index_t n) { return n; }

		LMAT_ENSURE_INLINE
		static index_t rem(index_t n) { return 0; }

		LMAT_ENSURE_INLINE
		static index_t maj(index_t n) { return n; }
	};

	LMAT_DEFINE_INT_DIV_C(1, 2)
	LMAT_DEFINE_INT_DIV_C(2, 4)
	LMAT_DEFINE_INT_DIV_C(3, 8)
	LMAT_DEFINE_INT_DIV_C(4, 16)
	LMAT_DEFINE_INT_DIV_C(5, 32)
	LMAT_DEFINE_INT_DIV_C(6, 64)
	LMAT_DEFINE_INT_DIV_C(7, 128)
	LMAT_DEFINE_INT_DIV_C(8, 256)
	LMAT_DEFINE_INT_DIV_C(9, 512)

	LMAT_DEFINE_INT_DIV_C(10, 1024)
}

#endif /* INT_DIV_H_ */
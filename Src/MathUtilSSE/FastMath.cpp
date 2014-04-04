//==============================================================================
//
//		FastMath.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/29/2008
//
//==============================================================================

#include "MathUtilSSE.h"

// Taken from: 
// http://ldesoras.free.fr/doc/articles/rounding_en.pdf
// See for details (ie, rounding to nearest even integer etc)

_NAMESPACE_BEGIN

_FORCE_INLINE int 
FastFloor(float val)
{
    __m128 xScalar = _mm_load_ss(&val);
    __m128 v2x = _mm_add_ss(xScalar, xScalar);
    v2x = _mm_add_ss(v2x, _mm_set_ss(-0.5f));

    // TODO: Must be in round-to-nearest mode
    int iv2x = _mm_cvtss_si32(v2x);

    // TODO: Compiler independent arithmetic shift
    iv2x = (iv2x >> 1);
    return iv2x;

	//const float ROUND_NEGATIVE_INFINITY = -0.5f;
	//
	//int i;
	//__asm
	//{
	//	fld		val
	//	fadd	st, st(0)
	//	fadd	ROUND_NEGATIVE_INFINITY
	//	fistp	i
	//	sar		i, 1
	//}
	//
	//return i;
}

_FORCE_INLINE int 
FastCeiling(float val)
{
	const float ROUND_POSITIVE_INFINITY = -0.5f;
	
	int i;
	__asm
	{
		fld		val
		fadd	st, st(0)
		fsubr	ROUND_POSITIVE_INFINITY
		fistp	i
		sar		i, 1
	}
	
	return -i;
}

_FORCE_INLINE int
FastRound(float val)
{
	const float ROUND_NEAREST = 0.5f;
	
	int i;
	__asm
	{
		fld		val
		fadd	st, st(0)
		fadd	ROUND_NEAREST
		fistp	i
		sar		i, 1
	}
	
	return i;
}

_FORCE_INLINE int
FastTruncate(float val)
{
	const float ROUND_NEGATIVE_INFINITY = -0.5f;
	
	int i;
	__asm
	{
		fld		val
		fadd	st, st(0)
		fabs
		fadd	ROUND_NEGATIVE_INFINITY
		fistp	i
		sar		i, 1
	}
	
	if(val < 0)
		i = -i;
	
	return i;
}

_NAMESPACE_END


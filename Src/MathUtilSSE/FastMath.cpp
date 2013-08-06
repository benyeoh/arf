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

inline int 
FastFloor(float val)
{
	const float ROUND_NEGATIVE_INFINITY = -0.5f;
	
	int i;
	__asm
	{
		fld		val
		fadd	st, st(0)
		fadd	ROUND_NEGATIVE_INFINITY
		fistp	i
		sar		i, 1
	}
	
	return i;
}

inline int 
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

inline int
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

inline int
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


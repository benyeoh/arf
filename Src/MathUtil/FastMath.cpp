//==============================================================================
//
//		FastMath.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/29/2008
//
//==============================================================================

#include "MathUtil.h"

_NAMESPACE_BEGIN

const static uint MAX_FACTORIAL = 13;
const static uint s_FactorialLookup[MAX_FACTORIAL] = 
{
	1,
	1,
	2,
	6,
	24,
	120,
	720,
	5040,
	40320,
	362880,
	3628800,
	39916800,
	479001600
};

inline uint
FastFactorial(uint val)
{
	if(val >= MAX_FACTORIAL)
		return gmtl::Math::factorial(val);

	return s_FactorialLookup[val];
}

_NAMESPACE_END
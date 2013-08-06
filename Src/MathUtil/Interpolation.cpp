//==============================================================================
//
//		Interpolate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		24/10/09
//
//==============================================================================

#include "MathUtil.h"

_NAMESPACE_BEGIN

float 
CubicInterpolate(float y0, float y1, float y2, float y3, float x)
{
	float x2 = x * x;
	float a0 = y3 - y2 - y0 + y1;
	float a1 = y0 - y1 - a0;
	float a2 = y2 - y0;
	float a3 = y1;

	return (a0 * x * x2 + a1 * x2 + a2 * x + a3);
}

_NAMESPACE_END

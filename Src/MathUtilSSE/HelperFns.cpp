//==============================================================================
//
//		HelperFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/22/2008
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

_FORCE_INLINE void 
LoadUnalignedMatrix44(const gmtl::Matrix44f* pMat, __m128 pData[4])
{
	pData[0] = _mm_loadu_ps(pMat->mData);
	pData[1] = _mm_loadu_ps(pMat->mData+4);
	pData[2] = _mm_loadu_ps(pMat->mData+8);
	pData[3] = _mm_loadu_ps(pMat->mData+12);
}

_NAMESPACE_END
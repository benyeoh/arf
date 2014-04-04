//==============================================================================
//
//		MatTranspose.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/26/2009
//
//==============================================================================

#include "MathUtilSSE.h"

_FORCE_INLINE gmtl::MatrixA44f* MatTranspose(const gmtl::MatrixA44f* pSrc, gmtl::MatrixA44f* pDest)
{
    __m128 col0 = _mm_load_ps( &(pSrc->mData[0]) );
    __m128 col1 = _mm_load_ps( &(pSrc->mData[4]) );
    __m128 col2 = _mm_load_ps( &(pSrc->mData[8]) );
    __m128 col3 = _mm_load_ps( &(pSrc->mData[12]) );

    _MM_TRANSPOSE4_PS(col0, col1, col2, col3);

    _mm_store_ps(&(pDest->mData[0]), col0);
    _mm_store_ps(&(pDest->mData[1]), col1);
    _mm_store_ps(&(pDest->mData[2]), col2);
    _mm_store_ps(&(pDest->mData[3]), col3);
}

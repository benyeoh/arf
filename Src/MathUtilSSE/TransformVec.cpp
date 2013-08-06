//==============================================================================
//
//		TransformVecSSE.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2008
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

inline gmtl::VecA4f* 
TransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut)
{
	__m128 xmm0, xmm1, xmm2, xmm3;
	//__m128 xmm4, xmm5, xmm6, xmm7;

	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	__m128& xmm4 = *((__m128*)(pMatData));		// col 0
	__m128& xmm5 = *((__m128*)(pMatData + 4));	// col 1
	__m128& xmm6 = *((__m128*)(pMatData + 8));	// col 2
	__m128& xmm7 = *((__m128*)(pMatData + 12));	// col 3

	xmm0 = _mm_load_ps1(pIn);		// Load and broadcast x
	xmm1 = _mm_load_ps1(pIn + 1);	// Load and broadcast y
	xmm2 = _mm_load_ps1(pIn + 2);	
	xmm3 = _mm_load_ps1(pIn + 3);

	xmm0 = _mm_mul_ps(xmm0, xmm4);
	xmm1 = _mm_mul_ps(xmm1, xmm5);	
	xmm2 = _mm_mul_ps(xmm2, xmm6);		
	xmm3 = _mm_mul_ps(xmm3, xmm7);

	xmm0 = _mm_add_ps(xmm0, xmm1);		
	xmm2 = _mm_add_ps(xmm2, xmm3);
	xmm0 = _mm_add_ps(xmm0, xmm2);

	_mm_store_ps(pOut, xmm0);

	return pVecOut;
}

inline gmtl::Vec4f* 
TransformVec(const gmtl::Matrix44f* pMat, const gmtl::Vec4f* pVecIn, gmtl::Vec4f* pVecOut)
{
	__m128 xmm0, xmm1, xmm2, xmm3;
	__m128 xmm4, xmm5, xmm6, xmm7;

	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	xmm4 = _mm_loadu_ps(pMatData);		// col 0
	xmm5 = _mm_loadu_ps(pMatData + 4);	// col 1
	xmm6 = _mm_loadu_ps(pMatData + 8);	// col 2
	xmm7 = _mm_loadu_ps(pMatData + 12);	// col 3

	xmm0 = _mm_load_ps1(pIn);		// Load and broadcast x
	xmm1 = _mm_load_ps1(pIn + 1);	// Load and broadcast y
	xmm2 = _mm_load_ps1(pIn + 2);	// Load and broadcast z
	xmm3 = _mm_load_ps1(pIn + 3);	// Load and broadcast w

	xmm0 = _mm_mul_ps(xmm0, xmm4);
	xmm1 = _mm_mul_ps(xmm1, xmm5);	
	xmm2 = _mm_mul_ps(xmm2, xmm6);		
	xmm3 = _mm_mul_ps(xmm3, xmm7);

	xmm0 = _mm_add_ps(xmm0, xmm1);		
	xmm2 = _mm_add_ps(xmm2, xmm3);
	xmm0 = _mm_add_ps(xmm0, xmm2);

	_mm_storeu_ps(pOut, xmm0);
	
	return pVecOut;
}

inline gmtl::VecA3f* 
TransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut)
{
	__m128 xmm0, xmm1, xmm2;
	
	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	__m128& xmm4 = *((__m128*)(pMatData));		// col 0
	__m128& xmm5 = *((__m128*)(pMatData + 4));	// col 1
	__m128& xmm6 = *((__m128*)(pMatData + 8));	// col 2
	
	xmm0 = _mm_load_ps1(pIn);		// Load and broadcast x
	xmm1 = _mm_load_ps1(pIn + 1);	// Load and broadcast y
	xmm2 = _mm_load_ps1(pIn + 2);	// Load and broadcast z
	
	xmm0 = _mm_mul_ps(xmm0, xmm4);
	xmm1 = _mm_mul_ps(xmm1, xmm5);	
	xmm2 = _mm_mul_ps(xmm2, xmm6);		
	
	xmm0 = _mm_add_ps(xmm0, xmm1);		
	xmm0 = _mm_add_ps(xmm0, xmm2);

	_mm_store_ps(pOut, xmm0);

	return pVecOut;
}


_NAMESPACE_END
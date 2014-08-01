//==============================================================================
//
//		MatOp.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2008
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

_FORCE_INLINE gmtl::MatrixA44f* 
MatMatMult(gmtl::MatrixA44f* pResult, const gmtl::MatrixA44f* pLHS, const gmtl::MatrixA44f* pRHS)
{
    __m128 xmm0, xmm1, xmm2, xmm3;
    //__m128 xmm4, xmm5, xmm6, xmm7;

    float* pLHSData = (float*) pLHS->mData;
    float* pRHSData = (float*) pRHS->mData;
    float* pResultData = (float*) pResult->mData;

    __m128& col0 = *((__m128*)(pLHSData));		// col 0
    __m128& col1 = *((__m128*)(pLHSData + 4));	// col 1
    __m128& col2 = *((__m128*)(pLHSData + 8));	// col 2
    __m128& col3 = *((__m128*)(pLHSData + 12));	// col 3

    // Process first 2 vertices	of the RHS matrix
    // Note that the matrix is in column major format and the vertices are in each column
    // Process x
    xmm0 = _mm_load_ps1(pRHSData);		// Load and broadcast x1
    xmm1 = _mm_load_ps1(pRHSData + 4);	// Load and broadcast x2
    xmm2 = _mm_mul_ps(xmm0, col0);
    xmm3 = _mm_mul_ps(xmm1, col0);

    // Process y
    xmm0 = _mm_load_ps1(pRHSData + 1);	// Load and broadcast y1
    xmm1 = _mm_load_ps1(pRHSData + 5);	// Load and broadcast y2
    xmm0 = _mm_mul_ps(xmm0, col1);
    xmm1 = _mm_mul_ps(xmm1, col1);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process z
    xmm0 = _mm_load_ps1(pRHSData + 2);	
    xmm1 = _mm_load_ps1(pRHSData + 6);
    xmm0 = _mm_mul_ps(xmm0, col2);
    xmm1 = _mm_mul_ps(xmm1, col2);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process w
    xmm0 = _mm_load_ps1(pRHSData + 3);	
    xmm1 = _mm_load_ps1(pRHSData + 7);
    xmm0 = _mm_mul_ps(xmm0, col3);
    xmm1 = _mm_mul_ps(xmm1, col3);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    _mm_store_ps(pResultData, xmm2);
    _mm_store_ps(pResultData + 4, xmm3);

    // Process the second 2 vertices	of the RHS matrix
    // Process x
    xmm0 = _mm_load_ps1(pRHSData + 8);	// Load and broadcast x3
    xmm1 = _mm_load_ps1(pRHSData + 12);	// Load and broadcast x4
    xmm2 = _mm_mul_ps(xmm0, col0);
    xmm3 = _mm_mul_ps(xmm1, col0);

    // Process y
    xmm0 = _mm_load_ps1(pRHSData + 9);	// Load and broadcast y3
    xmm1 = _mm_load_ps1(pRHSData + 13);	// Load and broadcast y4
    xmm0 = _mm_mul_ps(xmm0, col1);
    xmm1 = _mm_mul_ps(xmm1, col1);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process z
    xmm0 = _mm_load_ps1(pRHSData + 10);	// Load and broadcast z3
    xmm1 = _mm_load_ps1(pRHSData + 14);	// Load and broadcast z4
    xmm0 = _mm_mul_ps(xmm0, col2);
    xmm1 = _mm_mul_ps(xmm1, col2);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process w
    xmm0 = _mm_load_ps1(pRHSData + 11);	// Load and broadcast w3
    xmm1 = _mm_load_ps1(pRHSData + 15);	// Load and broadcast w4
    xmm0 = _mm_mul_ps(xmm0, col3);
    xmm1 = _mm_mul_ps(xmm1, col3);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    _mm_store_ps(pResultData + 8, xmm2);
    _mm_store_ps(pResultData + 12, xmm3);

    return pResult;
}

_FORCE_INLINE gmtl::Matrix44f* 
MatMatMult(gmtl::Matrix44f* pResult, const gmtl::Matrix44f* pLHS, const gmtl::Matrix44f* pRHS)
{
    __m128 xmm0, xmm1, xmm2, xmm3;
    __m128 xmm4, xmm5, xmm6, xmm7;

    float* pLHSData = (float*) pLHS->mData;
    float* pRHSData = (float*) pRHS->mData;
    float* pResultData = (float*) pResult->mData;

    xmm4 = _mm_loadu_ps(pLHSData);		// col 0
    xmm5 = _mm_loadu_ps(pLHSData + 4);	// col 1
    xmm6 = _mm_loadu_ps(pLHSData + 8);	// col 2
    xmm7 = _mm_loadu_ps(pLHSData + 12);	// col 3

    // Process first 2 vertices	of the RHS matrix
    // Note that the matrix is in column major format and the vertices are in each column
    // Process x
    xmm0 = _mm_load_ps1(pRHSData);		// Load and broadcast x1
    xmm1 = _mm_load_ps1(pRHSData + 4);	// Load and broadcast x2
    xmm2 = _mm_mul_ps(xmm0, xmm4);
    xmm3 = _mm_mul_ps(xmm1, xmm4);

    // Process y
    xmm0 = _mm_load_ps1(pRHSData + 1);	// Load and broadcast y1
    xmm1 = _mm_load_ps1(pRHSData + 5);	// Load and broadcast y2
    xmm0 = _mm_mul_ps(xmm0, xmm5);
    xmm1 = _mm_mul_ps(xmm1, xmm5);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process z
    xmm0 = _mm_load_ps1(pRHSData + 2);	
    xmm1 = _mm_load_ps1(pRHSData + 6);
    xmm0 = _mm_mul_ps(xmm0, xmm6);
    xmm1 = _mm_mul_ps(xmm1, xmm6);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process w
    xmm0 = _mm_load_ps1(pRHSData + 3);	
    xmm1 = _mm_load_ps1(pRHSData + 7);
    xmm0 = _mm_mul_ps(xmm0, xmm7);
    xmm1 = _mm_mul_ps(xmm1, xmm7);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    _mm_storeu_ps(pResultData, xmm2);
    _mm_storeu_ps(pResultData + 4, xmm3);

    // Process the second 2 vertices	of the RHS matrix
    // Process x
    xmm0 = _mm_load_ps1(pRHSData + 8);	// Load and broadcast x3
    xmm1 = _mm_load_ps1(pRHSData + 12);	// Load and broadcast x4
    xmm2 = _mm_mul_ps(xmm0, xmm4);
    xmm3 = _mm_mul_ps(xmm1, xmm4);

    // Process y
    xmm0 = _mm_load_ps1(pRHSData + 9);	// Load and broadcast y3
    xmm1 = _mm_load_ps1(pRHSData + 13);	// Load and broadcast y4
    xmm0 = _mm_mul_ps(xmm0, xmm5);
    xmm1 = _mm_mul_ps(xmm1, xmm5);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process z
    xmm0 = _mm_load_ps1(pRHSData + 10);	// Load and broadcast z3
    xmm1 = _mm_load_ps1(pRHSData + 14);	// Load and broadcast z4
    xmm0 = _mm_mul_ps(xmm0, xmm6);
    xmm1 = _mm_mul_ps(xmm1, xmm6);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    // Process w
    xmm0 = _mm_load_ps1(pRHSData + 11);	// Load and broadcast w3
    xmm1 = _mm_load_ps1(pRHSData + 15);	// Load and broadcast w4
    xmm0 = _mm_mul_ps(xmm0, xmm7);
    xmm1 = _mm_mul_ps(xmm1, xmm7);	
    xmm2 = _mm_add_ps(xmm2, xmm0);		
    xmm3 = _mm_add_ps(xmm3, xmm1);

    _mm_storeu_ps(pResultData + 8, xmm2);
    _mm_storeu_ps(pResultData + 12, xmm3);

    return pResult;
}

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

	return pDest;
}


_NAMESPACE_END
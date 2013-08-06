//==============================================================================
//
//		BatchTransformVecSSE.cpp
//
//			See: http://www.cortstratton.org/articles/HugiCode.html#bt1
//
//		Author:		Ben Yeoh
//		Date:		9/3/2007
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

void 
BatchTransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len)
{	
	__m128 xmm0, xmm1, xmm2, xmm3;
//	__m128 xmm4, xmm5, xmm6, xmm7;

	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	__m128& xmm4 = *((__m128*)pMatData);		// col 0
	__m128& xmm5 = *((__m128*)(pMatData + 4));	// col 1
	__m128& xmm6 = *((__m128*)(pMatData + 8));	// col 2
	__m128& xmm7 = *((__m128*)(pMatData + 12));	// col 3

	if(len & 0x1)
	{
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

		_mm_store_ps(pOut, xmm0);
		
		pIn+=4;
		pOut+=4;
	}
	
	len >>= 1;

	_LOOPi(len)	
	{	
		// Process 2 vertices at a time to take advantage of memory locality
		// Note that the matrix is in column major format and the vertices are in each column
		// Process x
		xmm0 = _mm_load_ps1(pIn);
		xmm1 = _mm_load_ps1(pIn + 4);
		xmm2 = _mm_mul_ps(xmm0, xmm4);
		xmm3 = _mm_mul_ps(xmm1, xmm4);

		// Process y
		xmm0 = _mm_load_ps1(pIn + 1);
		xmm1 = _mm_load_ps1(pIn + 5);
		xmm0 = _mm_mul_ps(xmm0, xmm5);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process z
		xmm0 = _mm_load_ps1(pIn + 2);
		xmm1 = _mm_load_ps1(pIn + 6);
		xmm0 = _mm_mul_ps(xmm0, xmm6);
		xmm1 = _mm_mul_ps(xmm1, xmm6);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process w
		xmm0 = _mm_load_ps1(pIn + 3);
		xmm1 = _mm_load_ps1(pIn + 7);
		xmm0 = _mm_mul_ps(xmm0, xmm7);
		xmm1 = _mm_mul_ps(xmm1, xmm7);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		_mm_store_ps(pOut, xmm2);
		_mm_store_ps(pOut + 4, xmm3);
		
		pIn += 8;
		pOut += 8;
	}
}

void 
BatchTransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut, uint len)
{	
	__m128 xmm0, xmm1, xmm2, xmm3;

	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	__m128& xmm4 = *((__m128*)pMatData);		// col 0, therefore the inv of the axis...
	__m128& xmm5 = *((__m128*)(pMatData + 4));	// col 1, therefore the inv of the axis...
	__m128& xmm6 = *((__m128*)(pMatData + 8));	// col 2, therefore the inv of the axis...
	
	if(len & 0x1)
	{
		xmm0 = _mm_load_ps1(pIn);		// Load and broadcast x
		xmm1 = _mm_load_ps1(pIn + 1);	// Load and broadcast y
		xmm2 = _mm_load_ps1(pIn + 2);	// Load and broadcast z
	
		xmm0 = _mm_mul_ps(xmm0, xmm4);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_mul_ps(xmm2, xmm6);		
	
		xmm0 = _mm_add_ps(xmm0, xmm1);		
		xmm0 = _mm_add_ps(xmm0, xmm2);

		_mm_store_ps(pOut, xmm0);

		pIn += 4;
		pOut += 4;
	}

	len >>= 1;
	
	_LOOPi(len)	
	{	
		// Process 2 vertices at a time to take advantage of memory locality
		// Note that the matrix is in column major format and the vertices are in each column
		// Process x
		xmm0 = _mm_load_ps1(pIn);
		xmm1 = _mm_load_ps1(pIn + 4);
		xmm2 = _mm_mul_ps(xmm0, xmm4);
		xmm3 = _mm_mul_ps(xmm1, xmm4);

		// Process y
		xmm0 = _mm_load_ps1(pIn + 1);
		xmm1 = _mm_load_ps1(pIn + 5);
		xmm0 = _mm_mul_ps(xmm0, xmm5);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process z
		xmm0 = _mm_load_ps1(pIn + 2);
		xmm1 = _mm_load_ps1(pIn + 6);
		xmm0 = _mm_mul_ps(xmm0, xmm6);
		xmm1 = _mm_mul_ps(xmm1, xmm6);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		_mm_store_ps(pOut, xmm2);
		_mm_store_ps(pOut + 4, xmm3);

		pIn += 8;
		pOut += 8;
	}
}

void
BatchTransformVec(const gmtl::Matrix44f* pMat, const gmtl::Vec4f* pVecIn, gmtl::Vec4f* pVecOut, uint len)
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

	if(len & 0x1)
	{
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

		pIn+=4;
		pOut+=4;
	}

	len >>= 1;

	_LOOPi(len)	
	{	
		// Process 2 vertices at a time to take advantage of memory locality
		// Note that the matrix is in column major format and the vertices are in each column
		// Process x
		xmm0 = _mm_load_ps1(pIn);
		xmm1 = _mm_load_ps1(pIn + 4);
		xmm2 = _mm_mul_ps(xmm0, xmm4);
		xmm3 = _mm_mul_ps(xmm1, xmm4);

		// Process y
		xmm0 = _mm_load_ps1(pIn + 1);
		xmm1 = _mm_load_ps1(pIn + 5);
		xmm0 = _mm_mul_ps(xmm0, xmm5);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process z
		xmm0 = _mm_load_ps1(pIn + 2);
		xmm1 = _mm_load_ps1(pIn + 6);
		xmm0 = _mm_mul_ps(xmm0, xmm6);
		xmm1 = _mm_mul_ps(xmm1, xmm6);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process w
		xmm0 = _mm_load_ps1(pIn + 3);
		xmm1 = _mm_load_ps1(pIn + 7);
		xmm0 = _mm_mul_ps(xmm0, xmm7);
		xmm1 = _mm_mul_ps(xmm1, xmm7);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		_mm_storeu_ps(pOut, xmm2);
		_mm_storeu_ps(pOut + 4, xmm3);

		pIn += 8;
		pOut += 8;
	}
}

void
BatchTransformAndProjectVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len)
{
	__m128 xmm0, xmm1, xmm2, xmm3;
	
	float* pMatData = (float*) pMat->mData;
	float* pIn = (float*) pVecIn;
	float* pOut = (float*) pVecOut;

	__m128& xmm4 = *((__m128*)pMatData);		// col 0
	__m128& xmm5 = *((__m128*)(pMatData + 4));	// col 1
	__m128& xmm6 = *((__m128*)(pMatData + 8));	// col 2
	__m128& xmm7 = *((__m128*)(pMatData + 12));	// col 3

	if(len & 0x1)
	{
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

		// Divide by w
		xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(3, 3, 3, 3));
		xmm1 = _mm_rcp_ps(xmm1);
		xmm0 = _mm_mul_ps(xmm0, xmm1);

		_mm_store_ps(pOut, xmm0);

		pIn+=4;
		pOut+=4;
	}

	len >>= 1;

	_LOOPi(len)	
	{	
		// Process 2 vertices at a time to take advantage of memory locality
		// Note that the matrix is in column major format and the vertices are in each column
		// Process x
		xmm0 = _mm_load_ps1(pIn);
		xmm1 = _mm_load_ps1(pIn + 4);
		xmm2 = _mm_mul_ps(xmm0, xmm4);
		xmm3 = _mm_mul_ps(xmm1, xmm4);

		// Process y
		xmm0 = _mm_load_ps1(pIn + 1);
		xmm1 = _mm_load_ps1(pIn + 5);
		xmm0 = _mm_mul_ps(xmm0, xmm5);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process z
		xmm0 = _mm_load_ps1(pIn + 2);
		xmm1 = _mm_load_ps1(pIn + 6);
		xmm0 = _mm_mul_ps(xmm0, xmm6);
		xmm1 = _mm_mul_ps(xmm1, xmm6);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process w
		xmm0 = _mm_load_ps1(pIn + 3);
		xmm1 = _mm_load_ps1(pIn + 7);
		xmm0 = _mm_mul_ps(xmm0, xmm7);
		xmm1 = _mm_mul_ps(xmm1, xmm7);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Do divide by w
		xmm0 = _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(3, 3, 3, 3));
		xmm1 = _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(3, 3, 3, 3));
		xmm0 = _mm_rcp_ps(xmm0);
		xmm1 = _mm_rcp_ps(xmm1);
		xmm2 = _mm_mul_ps(xmm0, xmm2);
		xmm3 = _mm_mul_ps(xmm1, xmm3);
		
		_mm_store_ps(pOut, xmm2);
		_mm_store_ps(pOut + 4, xmm3);

		pIn += 8;
		pOut += 8;
	}
}

_NAMESPACE_END
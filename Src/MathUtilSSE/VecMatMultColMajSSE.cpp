//==============================================================================
//
//		VecMatMultColMajSSE.cpp
//
//			See: http://www.cortstratton.org/articles/HugiCode.html#bt1
//
//		Author:		Ben Yeoh
//		Date:		9/4/2007
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

void 
VecMatMultColMaj(gmtl::MatrixA44f* pMat, gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut)
{
	// TODO: Use movaps
	// Get a pointer to the elements of m
	float* row0 = pMat->mData;

	__asm
	{
		mov         esi, pVecIn
		mov         edi, pVecOut

		// load columns of matrix into xmm4-7
		mov         edx, row0
		movups		xmm4, [edx]
		movups		xmm5, [edx+0x10]
		movups		xmm6, [edx+0x20]
		movups		xmm7, [edx+0x30]

		// load v into xmm0.
		movups		xmm0, [esi]

		// we'll store the final result in xmm2; initialize it
		// to zero
		xorps		xmm2, xmm2

		// broadcast x into xmm1, multiply it by the first
		// column of the matrix (xmm4), and add it to the total
		movups		xmm1, xmm0
		shufps		xmm1, xmm1, 0x00
		mulps		xmm1, xmm4
		addps		xmm2, xmm1

		// repeat the process for y, z and w
		movups		xmm1, xmm0
		shufps		xmm1, xmm1, 0x55
		mulps		xmm1, xmm5
		addps		xmm2, xmm1
		movups		xmm1, xmm0
		shufps		xmm1, xmm1, 0xAA
		mulps		xmm1, xmm6
		addps		xmm2, xmm1
		movups		xmm1, xmm0
		shufps		xmm1, xmm1, 0xFF
		mulps		xmm1, xmm7
		addps		xmm2, xmm1

		// write the results to vout
		movups		[edi], xmm2
	}
}

_NAMESPACE_END
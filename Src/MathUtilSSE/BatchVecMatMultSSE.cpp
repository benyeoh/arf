//==============================================================================
//
//		BatchVecMatMultSSE.cpp
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
BatchVecMatMult(gmtl::Matrix44f* pMat, gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len)
{	
	// TODO: Will moveaps work? Expects all structs to be 16 byte aligned
	
	// initializations in C++ land
	gmtl::MatrixA44f matTrans;
	gmtl::transpose(*(gmtl::Matrix44f*)(&matTrans), *pMat);
	float* row0 = matTrans.mData;
	static const int vecSize = 2 * sizeof(gmtl::VecA4f);

	// if there are an odd number of vectors, process the first one
	// separately and advance the pointers
	if (len & 0x1) 
	{
		VecMatMultColMaj(&matTrans, pVecIn, pVecOut);
		++pVecIn;
		++pVecOut;
	}
	len >>= 1; // we process two vectors at a time

	__asm 
	{
		mov         esi, pVecIn
		mov         edi, pVecOut
		mov         ecx, len
		
		// load columns of matrix into xmm4-7
		mov         edx, row0
		movaps		xmm4, [edx]
		movaps		xmm5, [edx+0x10]
		movaps		xmm6, [edx+0x20]
		movaps		xmm7, [edx+0x30]

BM5_START:

		// process x
		movss		xmm1, [esi+0x00]
		movss		xmm3, [esi+0x10]
		shufps		xmm1, xmm1, 0x00
		prefetchnta		[esi+0x30]
		shufps		xmm3, xmm3, 0x00
		mulps		xmm1, xmm4
		prefetchnta		[edi+0x30]
		mulps		xmm3, xmm4

		// process y
		movss		xmm0, [esi+0x04]
		movss		xmm2, [esi+0x14]
		shufps		xmm0, xmm0, 0x00
		shufps		xmm2, xmm2, 0x00
		mulps		xmm0, xmm5
		mulps		xmm2, xmm5
		addps		xmm1, xmm0
		addps		xmm3, xmm2

		// process z
		movss		xmm0, [esi+0x08]
		movss		xmm2, [esi+0x18]
		shufps		xmm0, xmm0, 0x00
		shufps		xmm2, xmm2, 0x00
		mulps		xmm0, xmm6
		mulps		xmm2, xmm6
		addps		xmm1, xmm0
		addps		xmm3, xmm2

		// process w (hiding some pointer increments between the
		// multiplies)
		movss		xmm0, [esi+0x0C]
		movss		xmm2, [esi+0x1C]
		shufps		xmm0, xmm0, 0x00
		shufps		xmm2, xmm2, 0x00
		mulps		xmm0, xmm7
		add         esi, vecSize
		mulps		xmm2, xmm7
		add         edi, vecSize
		addps		xmm1, xmm0
		addps		xmm3, xmm2

		// write output vectors to memory, and loop
		movaps		[edi-0x20], xmm1
		movaps		[edi-0x10], xmm3
		dec         ecx
		jnz         BM5_START
	}
}

_NAMESPACE_END
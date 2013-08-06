//==============================================================================
//
//		VecOp.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/26/2009
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

inline void
VecScalarMult(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, float val)
{
	__m128 splat = _mm_set1_ps(val);
	__m128& v1 = *((__m128*)(pV1));	// col 0
	
	__m128 res = _mm_mul_ps(v1, splat);
	_mm_store_ps((float*) pResult, res);
}

inline void
VecScalarMult(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, float val)
{
	VecScalarMult(&_CAST_VECA4(*pResult), &_CAST_VECA4(*pV1), val);
}

inline void
VecVecMult(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 res = _mm_mul_ps(v1, v2);
	
	_mm_store_ps((float*) pResult, res);
}

inline void
VecVecMult(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	VecVecMult(&_CAST_VECA4(*pResult), &_CAST_VECA4(*pV1), &_CAST_VECA4(*pV2));
}

inline void VecVecMult(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2)
{
	// SSE 4.1
	__m128i& v1 = *((__m128i*)(pV1));	// col 0
	__m128i& v2 = *((__m128i*)(pV2));	// col 1

	__m128i res = _mm_mullo_epi32(v1, v2);

	_mm_store_si128((__m128i*) pResult, res);
}


inline void
VecVecDiv(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 rcp	= _mm_rcp_ps(v2);
	__m128 res	= _mm_mul_ps(v1, rcp);
	
	_mm_store_ps((float*) pResult, res);
}

inline void
VecVecDiv(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	VecVecDiv(&_CAST_VECA4(*pResult), &_CAST_VECA4(*pV1), &_CAST_VECA4(*pV2));
}

inline void VecScalarAdd(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, float val)
{
	__m128 splat = _mm_set1_ps(val);
	__m128& v1 = *((__m128*)(pV1));	// col 0

	__m128 res = _mm_add_ps(v1, splat);
	_mm_store_ps((float*) pResult, res);
}

inline void
VecVecAdd(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 res = _mm_add_ps(v1, v2);
	
	_mm_store_ps((float*) pResult, res);

}

inline void VecVecAdd(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	VecVecAdd(&_CAST_VECA4(*pResult), &_CAST_VECA4(*pV1), &_CAST_VECA4(*pV2));
}

inline void VecVecAdd(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2)
{
	__m128i& v1 = *((__m128i*)(pV1));	// col 0
	__m128i& v2 = *((__m128i*)(pV2));	// col 1

	__m128i res = _mm_add_epi32(v1, v2);

	_mm_store_si128((__m128i*) pResult, res);
}

inline void VecVecAdd(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
	VecVecAdd(&_CAST_VECA4I(*pResult), &_CAST_VECA4I(*pV1), &_CAST_VECA4I(*pV2));
}


inline void
VecVecSub(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 res = _mm_sub_ps(v1, v2);
	
	_mm_store_ps((float*) pResult, res);

}

inline void VecVecSub(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	VecVecSub(&_CAST_VECA4(*pResult), &_CAST_VECA4(*pV1), &_CAST_VECA4(*pV2));
}

inline void VecVecSub(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2)
{
	__m128i& v1 = *((__m128i*)(pV1));	// col 0
	__m128i& v2 = *((__m128i*)(pV2));	// col 1

	__m128i res = _mm_sub_epi32(v1, v2);
	_mm_store_si128((__m128i*) pResult, res);
}

inline void VecVecSub(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
	VecVecSub(&_CAST_VECA4I(*pResult), &_CAST_VECA4I(*pV1), &_CAST_VECA4I(*pV2));
}

inline void
VecVecDot(float* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 prod		= _mm_mul_ps(v1, v2);
	__m128 zFirst	= _mm_movehl_ps(prod, prod);
	__m128 yFirst	= _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(1, 1, 1, 1));
	prod			= _mm_add_ss(prod, zFirst);
	prod			= _mm_add_ss(prod, yFirst);
	
	_mm_store_ss(pResult, prod);
}

inline void
VecLength(float* pResult, const gmtl::VecA3f* pVec)
{
	__m128& v1 = *((__m128*)(pVec));	// col 0
	
	// Dot product
	__m128 prod		= _mm_mul_ps(v1, v1);
	__m128 zFirst	= _mm_movehl_ps(prod, prod);
	__m128 yFirst	= _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(1, 1, 1, 1));
	prod			= _mm_add_ss(prod, zFirst);
	prod			= _mm_add_ss(prod, yFirst);
	
	// Fast approx sqrt
	prod			= _mm_rsqrt_ss(prod);
	prod			= _mm_rcp_ss(prod);
	
	_mm_store_ss(pResult, prod);
}

inline gmtl::VecA3f*
NormalizeVec(gmtl::VecA3f* pResult, const gmtl::VecA3f* pIn)
{
	__m128 vec	= _mm_load_ps((float*) pIn->mData);

	__m128 vecSq	= _mm_mul_ps(vec, vec);
	__m128 zFirst	= _mm_movehl_ps(vecSq, vecSq);
	__m128 yFirst	= _mm_shuffle_ps(vecSq, vecSq, _MM_SHUFFLE(1, 1, 1, 1));
	vecSq			= _mm_add_ss(vecSq, zFirst);
	vecSq			= _mm_add_ss(vecSq, yFirst);

	__m128 rcpLength	= _mm_rsqrt_ss(vecSq);
	rcpLength			= _mm_shuffle_ps(rcpLength, rcpLength, _MM_SHUFFLE(0, 0, 0, 0));
	vec					= _mm_mul_ps(vec, rcpLength);
	
	_mm_store_ps(pResult->mData, vec);
	
	return pResult;
}

inline void
NormalizeVecLength(float* pLength, gmtl::VecA3f* pResult, const gmtl::VecA3f* pIn)
{
	__m128 vec	= _mm_load_ps((float*) pIn->mData);

	__m128 vecSq	= _mm_mul_ps(vec, vec);
	__m128 zFirst	= _mm_movehl_ps(vecSq, vecSq);
	__m128 yFirst	= _mm_shuffle_ps(vecSq, vecSq, _MM_SHUFFLE(1, 1, 1, 1));
	vecSq			= _mm_add_ss(vecSq, zFirst);
	vecSq			= _mm_add_ss(vecSq, yFirst);

	__m128 rcpLength	= _mm_rsqrt_ss(vecSq);
	__m128 length		= _mm_rcp_ss(rcpLength);
	
	_mm_store_ss(pLength, length);
	
	rcpLength			= _mm_shuffle_ps(rcpLength, rcpLength, _MM_SHUFFLE(0, 0, 0, 0));
	vec					= _mm_mul_ps(vec, rcpLength);
	
	_mm_store_ps(pResult->mData, vec);
}

inline void
VecCrossNormalize(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1
	
	__m128 v1yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(0, 0, 2, 1));
	__m128 v2zxy = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1, 1, 0, 2));
	__m128 v1zxy = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1, 1, 0, 2));
	__m128 v2yzx = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(0, 0, 2, 1));
	
	__m128 temp1 = _mm_mul_ps(v1yzx, v2zxy);
	__m128 temp2 = _mm_mul_ps(v1zxy, v2yzx);
	temp1 = _mm_sub_ps(temp1, temp2);
	
	// Normalize
	__m128 vecSq	= _mm_mul_ps(temp1, temp1);
	__m128 zFirst	= _mm_movehl_ps(vecSq, vecSq);
	__m128 yFirst	= _mm_shuffle_ps(vecSq, vecSq, _MM_SHUFFLE(1, 1, 1, 1));
	vecSq			= _mm_add_ss(vecSq, zFirst);
	vecSq			= _mm_add_ss(vecSq, yFirst);

	__m128 rcpLength	= _mm_rsqrt_ss(vecSq);
	rcpLength			= _mm_shuffle_ps(rcpLength, rcpLength, _MM_SHUFFLE(0, 0, 0, 0));
	temp1				= _mm_mul_ps(temp1, rcpLength);
	
	_mm_store_ps((float*) pResult, temp1);
}

inline void
VecVecSlerp(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2, float t)
{
	gmtl::VecA3f basis;
	
	const static float _THRESHOLD = 0.87f;
	float dot;
	VecVecDot(&dot, pV1, pV2);
	if(dot > _THRESHOLD)
	{
		gmtl::VecA3f diff;
		VecVecSub((gmtl::VecA4f*)&diff, (gmtl::VecA4f*) pV2, (gmtl::VecA4f*)pV1);
		VecScalarMult((gmtl::VecA4f*)&diff, (gmtl::VecA4f*)&diff, t);
		VecVecAdd((gmtl::VecA4f*)pResult, (gmtl::VecA4f*)&diff, (gmtl::VecA4f*)pV1);
		
		NormalizeVec(pResult, pResult);
		return;
	}
	else if(dot < -_THRESHOLD)
	{
		float upDot = gmtl::Math::abs((*pV1)[1]);
		if(upDot > _THRESHOLD)
			basis.set(1.0f, 0.0f, 0.0f);
		else
		{
			VecCrossNormalize(&basis, pV1, &gmtl::VecA3f(0.0f, 1.0f, 0.0f));
		}	
	}
	else
	{
		// Find the orthonormal basis
		gmtl::VecA3f projection;
		VecScalarMult((gmtl::VecA4f*) &projection,(gmtl::VecA4f*) pV1, dot);
		VecVecSub((gmtl::VecA4f*) &basis,(gmtl::VecA4f*) pV2, (gmtl::VecA4f*)&projection);
		NormalizeVec(&basis, &basis);
	}
		
	float angleBetween	= gmtl::Math::aCos(dot);
	float theta			= angleBetween * t;
	
	float rot1 = gmtl::Math::cos(theta);
	float rot2 = gmtl::Math::sin(theta);
	
	gmtl::VecA3f proj1;
	gmtl::VecA3f proj2;
	VecScalarMult((gmtl::VecA4f*)&proj1, (gmtl::VecA4f*)pV1, rot1);
	VecScalarMult((gmtl::VecA4f*)&proj2,(gmtl::VecA4f*) &basis, rot2);
	
	VecVecAdd((gmtl::VecA4f*)pResult, (gmtl::VecA4f*)&proj1, (gmtl::VecA4f*)&proj2);
}

inline void
VecVecMin(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1

	__m128 res = _mm_min_ps(v1, v2);
	_mm_store_ps((float*) pResult, res);
}

inline void VecVecMin(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
    __m128i& v1 = *((__m128i*)(pV1));	// col 0
    __m128i& v2 = *((__m128i*)(pV2));	// col 1

    // SSE4
    __m128i res = _mm_min_epi32(v1, v2);
    _mm_store_si128((__m128i*)pResult, res);
}

inline void VecVecMax(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
    __m128i& v1 = *((__m128i*)(pV1));	// col 0
    __m128i& v2 = *((__m128i*)(pV2));	// col 1

    // SSE4
    __m128i res = _mm_max_epi32(v1, v2);
    _mm_store_si128((__m128i*)pResult, res);
}

inline void
VecVecMax(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2)
{
	__m128& v1 = *((__m128*)(pV1));	// col 0
	__m128& v2 = *((__m128*)(pV2));	// col 1

	__m128 res = _mm_max_ps(v1, v2);
	_mm_store_ps((float*) pResult, res);
}

inline void VecFloor(gmtl::VecA3i* pResult, const gmtl::VecA3f* pV1)
{
	VecFloor((gmtl::VecA4i*) pResult, (const gmtl::VecA4f*) pV1);
}

inline void VecFloor(gmtl::VecA4i* pResult, const gmtl::VecA4f* pV1)
{
    __m128& v =  *((__m128*)(pV1));
    __m128 v2x = _mm_add_ps(v, v);
    v2x = _mm_add_ps(v2x, _mm_set1_ps(-0.5f));

    // TODO: Must be in round-to-nearest mode
    __m128i iv2x = _mm_cvtps_epi32(v2x);
    __m128i ires = _mm_srai_epi32(iv2x, 1);

    _mm_store_si128((__m128i*)pResult, ires);

	//__m128& v =  *((__m128*)(pV1));
	//__m128i iv = _mm_cvttps_epi32(v);
 //   __m128 res = _mm_cvtepi32_ps(iv);
 //   
 //   __m128i ivShifted = _mm_srli_epi32(*((__m128i*)&v), 31);
	//iv = _mm_sub_epi32(iv, ivShifted);

	//_mm_store_si128((__m128i*)pResult, iv);
}

inline void VecFloor(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1)
{
	VecFloor((gmtl::VecA4f*) pResult, (const gmtl::VecA4f*) pV1);
}

inline void VecFloor(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1)
{
    __m128& v =  *((__m128*)(pV1));
    __m128 v2x = _mm_add_ps(v, v);
    v2x = _mm_add_ps(v2x, _mm_set1_ps(-0.5f));

    // TODO: Must be in round-to-nearest mode
    __m128i iv2x = _mm_cvtps_epi32(v2x);
    __m128i ires = _mm_srai_epi32(iv2x, 1);

    __m128 res = _mm_cvtepi32_ps(ires);
    _mm_store_ps((float*)pResult, res);

	//__m128& v =  *((__m128*)(pV1));
	//__m128i iv = _mm_cvttps_epi32(v);
	//__m128i ivShifted = _mm_srli_epi32(*((__m128i*)&v), 31);
 //   iv = _mm_sub_epi32(iv, ivShifted);

	//__m128 res = _mm_cvtepi32_ps(iv);
	//_mm_store_ps((float*)pResult, res);
}

inline void VecIntToFloat(gmtl::VecA3f* pDest, const gmtl::VecA3i* pV1)
{
	VecIntToFloat((gmtl::VecA4f*) pDest, (const gmtl::VecA4i*) pV1);
}

inline void VecIntToFloat(gmtl::VecA4f* pDest, const gmtl::VecA4i* pV1)
{
	__m128 res = _mm_cvtepi32_ps(*((__m128i*)pV1));
	_mm_store_ps((float*)pDest, res);
}

inline void VecFloatToInt(gmtl::VecA3i* pDest, const gmtl::VecA3f* pV1)
{
	VecFloatToInt((gmtl::VecA4i*) pDest, (const gmtl::VecA4f*) pV1);
}

inline void VecFloatToInt(gmtl::VecA4i* pDest, const gmtl::VecA4f* pV1)
{
	__m128i res = _mm_cvttps_epi32(*((__m128*)pV1));
	_mm_store_si128((__m128i*)pDest, res);
}

inline void VecVecOr(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
	VecVecOr((gmtl::VecA4i*) pDest, (const gmtl::VecA4i*) pV1, (const gmtl::VecA4i*) pV2);
}

inline void VecVecOr(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2)
{
	__m128i& v1 = *((__m128i*)(pV1));
	__m128i& v2 = *((__m128i*)(pV2));
	__m128i res = _mm_or_si128(v1, v2);

	_mm_store_si128((__m128i*)pDest, res);
}

inline void VecScalarAnd(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, uint scalar)
{
	VecScalarAnd((gmtl::VecA4i*) pResult, (const gmtl::VecA4i*) pV1, scalar);
}

inline void VecScalarAnd(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, uint scalar)
{
	__m128i& v1 = *((__m128i*)(pV1));
	__m128i v2 = _mm_set1_epi32((int) scalar);
	__m128i res = _mm_and_si128(v1, v2);

	_mm_store_si128((__m128i*)pResult, res);
}

inline void VecScalarShiftLL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, uint scalar)
{
	VecScalarShiftLL((gmtl::VecA4i*) pDest, (const gmtl::VecA4i*) pV1, scalar);
}

inline void VecScalarShiftLL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, uint scalar)
{
	__m128i& v1 = *((__m128i*)(pV1));
	__m128i res = _mm_slli_epi32(v1, (int) scalar);

	_mm_store_si128((__m128i*)pDest, res);
}

inline void VecScalarShiftRL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, uint scalar)
{
	VecScalarShiftRL((gmtl::VecA4i*) pDest, (const gmtl::VecA4i*) pV1, scalar);
}

inline void VecScalarShiftRL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, uint scalar)
{
	__m128i& v1 = *((__m128i*)(pV1));
	__m128i res = _mm_srli_epi32(v1, (int) scalar);

	_mm_store_si128((__m128i*)pDest, res);
}

inline void VecVecShiftRL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2)
{
	VecVecShiftRL((gmtl::VecA4i*) pDest, (const gmtl::VecA4i*) pV1, (const gmtl::VecA4i*) pV2);
}

inline void VecVecShiftRL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2)
{
    // TODO: Requires AVX
    _DEBUG_ASSERT(FALSE);
	//__m128i& v1 = *((__m128i*)(pV1));
	//__m128i& v2 = *((__m128i*)(pV2));

	//__m128i res = _mm_srl_epi32(v1, v2);

	//_mm_store_si128((__m128i*)pDest, res);
}

_NAMESPACE_END
//==============================================================================
//
//		VecOp.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/26/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

extern inline void
VecCrossNormalize(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);

extern inline void VecScalarMult(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, float val);
extern inline void VecScalarMult(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, float val);

extern inline void VecVecMult(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2);
extern inline void VecVecMult(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecMult(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2);

extern inline void VecVecDiv(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2);
extern inline void VecVecDiv(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);

extern inline void VecScalarAdd(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, float val);
extern inline void VecVecAdd(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2);
extern inline void VecVecAdd(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecAdd(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2);
extern inline void VecVecAdd(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

extern inline void VecVecSub(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1, const gmtl::VecA4f* pV2);
extern inline void VecVecSub(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecSub(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2);
extern inline void VecVecSub(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

extern inline void VecVecDot(float* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecDot(float* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);

extern inline gmtl::VecA3f* NormalizeVec(gmtl::VecA3f* pResult, const gmtl::VecA3f* pIn);

inline gmtl::VecA3f& NormalizeVec(gmtl::VecA3f& vec)
{
	return *NormalizeVec(&vec, &vec);
}

extern inline void VecLength(float* pResult, const gmtl::VecA3f* pVec);

extern inline void NormalizeVecLength(float* pLength, gmtl::VecA3f* pResult, const gmtl::VecA3f* pIn);

extern inline void VecVecSlerp(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2, float t);

extern inline void VecVecMin(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecMin(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

extern inline void VecVecMax(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1, const gmtl::VecA3f* pV2);
extern inline void VecVecMax(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

extern inline void VecFloor(gmtl::VecA4f* pResult, const gmtl::VecA4f* pV1);
extern inline void VecFloor(gmtl::VecA3f* pResult, const gmtl::VecA3f* pV1);
extern inline void VecFloor(gmtl::VecA4i* pResult, const gmtl::VecA4f* pV1);
extern inline void VecFloor(gmtl::VecA3i* pResult, const gmtl::VecA3f* pV1);

extern inline void VecIntToFloat(gmtl::VecA4f* pDest, const gmtl::VecA4i* pV1);
extern inline void VecIntToFloat(gmtl::VecA3f* pDest, const gmtl::VecA3i* pV1);

extern inline void VecFloatToInt(gmtl::VecA4i* pDest, const gmtl::VecA4f* pV1);
extern inline void VecFloatToInt(gmtl::VecA3i* pDest, const gmtl::VecA3f* pV1);

extern inline void VecVecOr(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2);
extern inline void VecVecOr(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

extern inline void VecScalarAnd(gmtl::VecA4i* pResult, const gmtl::VecA4i* pV1, uint scalar);
extern inline void VecScalarAnd(gmtl::VecA3i* pResult, const gmtl::VecA3i* pV1, uint scalar);

extern inline void VecScalarShiftLL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, uint scalar);
extern inline void VecScalarShiftLL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, uint scalar);

extern inline void VecScalarShiftRL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, uint scalar);
extern inline void VecScalarShiftRL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, uint scalar);

extern inline void VecVecShiftRL(gmtl::VecA4i* pDest, const gmtl::VecA4i* pV1, const gmtl::VecA4i* pV2);
extern inline void VecVecShiftRL(gmtl::VecA3i* pDest, const gmtl::VecA3i* pV1, const gmtl::VecA3i* pV2);

_NAMESPACE_END

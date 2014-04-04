//==============================================================================
//
//		BoundsSSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/22/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

_FORCE_INLINE void _TransformAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut);

_FORCE_INLINE void _TransformCenteredAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut);

_FORCE_INLINE void _TransformOOBoxToPoints(__m128* pMat, const OOBox* pIn, __m128* pOut);//gmtl::VecA4f* pOut);

_FORCE_INLINE void _TransformAABoxToPoints(__m128* pMat, const AABox* pIn, __m128* pOut);

_FORCE_INLINE void _TransformOOBoxToAABox(__m128* pMat, const OOBox* pIn, AABox* pOut);

_FORCE_INLINE void _TransformAABoxToAABox(__m128* pMat, const AABox* pIn, AABox* pOut);

_FORCE_INLINE void _TransformAndProjectOOBoxToAABox(__m128* pMat, float nearPlane, float farPlane, const OOBox* pIn, AABox* pOut);

_FORCE_INLINE void _TransformAndProjectAABoxToAABox(__m128* pMat, float nearPlane, float farPlane, const AABox* pIn, AABox* pOut);

_FORCE_INLINE boolean _AABoxInAABox(const AABox* pBox1, const AABox* pBox2);

_NAMESPACE_END
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

inline void
_TransformAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut);

inline void
_TransformCenteredAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut);

inline void
_TransformOOBoxToPoints(__m128* pMat, const OOBox* pIn, gmtl::VecA4f* pOut);

inline void
_TransformOOBoxToAABox(__m128* pMat, const OOBox* pIn, AABox* pOut);

inline void
_TransformAABoxToAABox(__m128* pMat, const AABox* pIn, AABox* pOut);

inline void
_TransformAndProjectOOBoxToAABox(__m128* pMat, float nearPlane, float farPlane, const OOBox* pIn, AABox* pOut);

inline void
_TransformAndProjectAABoxToAABox(__m128* pMat, float nearPlane, float farPlane, const AABox* pIn, AABox* pOut);

inline boolean
_AABoxInAABox(const AABox* pBox1, const AABox* pBox2);

_NAMESPACE_END
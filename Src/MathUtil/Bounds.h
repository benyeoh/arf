//==============================================================================
//
//		Bounds.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/11/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct Sphere
{
	gmtl::VecA4f center;
};

struct AABox
{
	gmtl::VecA3f min;
	gmtl::VecA3f max;
};

struct OOBox
{
	gmtl::VecA3f corner;
	gmtl::VecA3f xExtents;
	gmtl::VecA3f yExtents;
	gmtl::VecA3f zExtents;
};


extern inline void
TransformAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut);

extern inline void
TransformCenteredAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut);

extern inline void
TransformOOBoxToPoints(const gmtl::MatrixA44f* pMat, const OOBox* pIn, gmtl::VecA4f* pOut);

extern inline void
TransformOOBoxToAABox(const gmtl::MatrixA44f* pMat, const OOBox* pIn, AABox* pOut);

extern inline void
TransformAABoxToAABox(const gmtl::MatrixA44f* pMat, const AABox* pIn, AABox* pOut);

extern inline void
TransformAndProjectOOBoxToAABox(const gmtl::MatrixA44f* pMat, float nearPlane, float farPlane, const OOBox* pIn, AABox* pOut);

extern inline void
TransformAndProjectAABoxToAABox(const gmtl::MatrixA44f* pMat, float nearPlane, float farPlane, const AABox* pIn, AABox* pOut);

extern inline void
BatchTransformAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut, int length);

extern inline void
BatchTransformCenteredAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut, int length);

extern inline void
BatchTransformOOBoxToPoints(const gmtl::MatrixA44f* pMat, const OOBox* pIn, gmtl::VecA3f* pOut, int length);

//extern inline void
//BatchTransformOOBoxToAABox(const gmtl::MatrixA44f* pMat, const OOBox* pIn, gmtl::VecA3f* pOut, int length);
//
//extern inline void
//BatchTransformAndProjectOOBoxToAABoxPreserveZ(const gmtl::Matrix44f* pMat, const OOBox* pIn, AABox* pOut, int length);

extern inline void
OOBoxToPoints(const OOBox* pIn, gmtl::VecA3f* pOut);

extern inline void
OOBoxToAABox(const OOBox* pIn, AABox* pOut);

extern inline void
PointsToAABox(const gmtl::VecA3f* pIn, int length, AABox* pOut);

extern inline void
PointsToAABox(const gmtl::Vec4f* pIn, int length, int stride, AABox* pOut);

extern inline void
OOBoxPointsToAABox(const gmtl::VecA3f* pIn, AABox* pOut);

extern inline boolean
AABoxInAABox(const AABox* pBox1, const AABox* pBox2);

extern inline boolean
AABoxContainsAABox(const AABox* pContainer, const AABox* pContainee);

extern inline void
AABoxMerge(AABox* pResult, const AABox* pBox1, const AABox* pBox2);

extern inline boolean
AABoxInSphere(const AABox* pBox, const Sphere* pSphere);

extern inline boolean SphereInAABox(const Sphere* pSphere, const AABox* pBox);
extern inline boolean SphereContainsAABox(const Sphere* pSphere, const AABox* pBox);

extern inline float AABoxToPointDist(const AABox* pBox, const gmtl::VecA3f* pPoint);

_NAMESPACE_END
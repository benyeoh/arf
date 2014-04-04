//==============================================================================
//
//		BoundsSSE.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/20/2008
//
//==============================================================================

#include "MathUtilSSE.h"

_NAMESPACE_BEGIN

_FORCE_INLINE void BatchTransformAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut, int length)
{
	//__m128 mat[4];

	//LoadMatrix44(pMat, mat);
	
	_LOOPi(length)
	{	
		_TransformAABoxToOOBox((__m128*) pMat, pIn, pOut);
		pIn++;
		pOut++;
	}
}

_FORCE_INLINE void
BatchTransformCenteredAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut, int length)
{
	//__m128 mat[4];

	//LoadMatrix44(pMat, mat);

	_LOOPi(length)
	{	
		_TransformCenteredAABoxToOOBox((__m128*) pMat, pIn, pOut);
		pIn++;
		pOut++;
	}
}

_FORCE_INLINE void
BatchTransformOOBoxToPoints(const gmtl::MatrixA44f* pMat, const OOBox* pIn, gmtl::VecA4f* pOut, int length)
{
	//__m128 mat[4];

	//LoadMatrix44(pMat, mat);

	_LOOPi(length)
	{	
		_TransformOOBoxToPoints((__m128*) pMat, pIn, (__m128*) pOut);
		pIn++;
		pOut+=8;
	}
}

//inline void
//BatchTransformAndProjectOOBoxToAABox(const gmtl::Matrix44f* pMat, const OOBox* pIn, AABox* pOut, int length)
//{
//	__m128 mat[4];
//
//	LoadMatrix44(pMat, mat);
//
//	_LOOPi(length)
//	{	
//		_TransformAndProjectOOBoxToAABox(mat, pIn, pOut);
//		pIn++;
//		pOut++;
//	}
//}

_FORCE_INLINE void TransformAABoxToPoints(const gmtl::MatrixA44f* pMat, const AABox* pIn, gmtl::VecA4f* pOut)
{
    _TransformAABoxToPoints((__m128*) pMat, pIn, (__m128*) pOut);
}


_FORCE_INLINE void
_TransformAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut)
{
	// Assumes w = 1.0f and pMat is NOT a projection matrix
	__m128 xmm0, xmm1, xmm2, xmm3;
	//__m128 col0, col1, col2, trans;

	float* pMinData = (float*) pIn->min.mData;
	float* pMaxData = (float*) pIn->max.mData;	

	//// Zero the reserved w component of the aligned vector
	//pMinData[4] = 0.0f;
	//pMaxData[4] = 0.0f;

	__m128& col0 = pMat[0];
	__m128& col1 = pMat[1];
	__m128& col2 = pMat[2];
	__m128& trans = pMat[3];

	__m128& max = *((__m128*)(pMaxData));
	__m128& min = *((__m128*)(pMinData));

	// Get the extents 
	__m128 extents = _mm_sub_ps(max, min);

	// Broadcast the corner point
	xmm2 = _mm_shuffle_ps(min, min, _MM_SHUFFLE(1, 1, 1, 1));
	xmm3 = _mm_shuffle_ps(min, min, _MM_SHUFFLE(2, 2, 2, 2));
	xmm0 = _mm_shuffle_ps(min, min, _MM_SHUFFLE(0, 0, 0, 0));

	// Transform the corner point
	xmm0 = _mm_mul_ps(xmm0, col0);
	xmm2 = _mm_mul_ps(xmm2, col1);
	xmm3 = _mm_mul_ps(xmm3, col2);
	xmm0 = _mm_add_ps(xmm0, xmm3);
	xmm2 = _mm_add_ps(xmm2, trans);
	xmm3 = _mm_add_ps(xmm0, xmm2);

	// Broadcast the extents
	xmm0 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(0, 0, 0, 0));
	xmm2 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(2, 2, 2, 2));
	xmm1 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(1, 1, 1, 1));

	// Transform the extents
	xmm0 = _mm_mul_ps(xmm0, col0);
	xmm1 = _mm_mul_ps(xmm1, col1);
	xmm2 = _mm_mul_ps(xmm2, col2);

	_mm_store_ps(pOut->corner.mData, xmm3);
	_mm_store_ps(pOut->xExtents.mData, xmm0);
	_mm_store_ps(pOut->yExtents.mData, xmm1);
	_mm_store_ps(pOut->zExtents.mData, xmm2);
}

_FORCE_INLINE void
TransformAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut)
{
//	__m128 mat[4];
	
//	LoadMatrix44(pMat, mat);
	_TransformAABoxToOOBox((__m128*) pMat, pIn, pOut);
}

_FORCE_INLINE void
_TransformCenteredAABoxToOOBox(__m128* pMat, const AABox* pIn, OOBox* pOut)
{
	// Assumes w = 1.0f and pMat is NOT a projection matrix
	// Also assumes that the bounding box is centered with the minimum at (0, 0, 0)
	__m128 xmm0, xmm1, xmm2;
	//__m128 col0, col1, col2, trans;

	float* pMinData = (float*) pIn->min.mData;
	float* pMaxData = (float*) pIn->max.mData;	

	// Load the extents
	__m128& extents = *((__m128*)(pMaxData));

	// Broadcast the extents	
	xmm0 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(0, 0, 0, 0));
	xmm2 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(2, 2, 2, 2));
	xmm1 = _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(1, 1, 1, 1));

	// Transform the extents
	xmm0 = _mm_mul_ps(xmm0, pMat[0]);
	xmm1 = _mm_mul_ps(xmm1, pMat[1]);
	xmm2 = _mm_mul_ps(xmm2, pMat[2]);

	_mm_store_ps(pOut->corner.mData, pMat[3]);
	_mm_store_ps(pOut->xExtents.mData, xmm0);
	_mm_store_ps(pOut->yExtents.mData, xmm1);
	_mm_store_ps(pOut->zExtents.mData, xmm2);
}

_FORCE_INLINE void
TransformCenteredAABoxToOOBox(const gmtl::MatrixA44f* pMat, const AABox* pIn, OOBox* pOut)
{
//	__m128 mat[4];

//	LoadMatrix44(pMat, mat);
	_TransformCenteredAABoxToOOBox((__m128*) pMat, pIn, pOut);
}

_FORCE_INLINE void
TransformOOBoxToAABox(const gmtl::MatrixA44f* pMat, const OOBox* pIn, AABox* pOut)
{
	_TransformOOBoxToAABox((__m128*) pMat, pIn, pOut);	
}

_FORCE_INLINE void
_TransformOOBoxToAABox(__m128* pMat, const OOBox* pIn, AABox* pOut)
{
    __m128 points[8];
    _TransformOOBoxToPoints(pMat, pIn, points);

    __m128 transformedCorner = points[0];
    __m128 dx = points[1]; // dx
    __m128 dy = points[2]; // dy
    __m128 dxdy = points[3]; // dx + dy

    __m128 dz = points[4]; // dz
    __m128 dxdz = points[5]; // dx + dz
    __m128 dydz = points[6]; // dy + dz
    __m128 dxdydz = points[7]; // dx + dy + dz
	
	// Now find the AABox
	__m128 min = _mm_min_ps(transformedCorner, dx);
	__m128 max = _mm_max_ps(transformedCorner, dx);
	min = _mm_min_ps(min, dy);
	max = _mm_max_ps(max, dy);
	min = _mm_min_ps(min, dxdy);
	max = _mm_max_ps(max, dxdy);
	min = _mm_min_ps(min, dz);
	max = _mm_max_ps(max, dz);
	min = _mm_min_ps(min, dxdz);
	max = _mm_max_ps(max, dxdz);
	min = _mm_min_ps(min, dydz);
	max = _mm_max_ps(max, dydz);
	min = _mm_min_ps(min, dxdydz);
	max = _mm_max_ps(max, dxdydz);

	_mm_store_ps(pOut->min.mData, min);	
	_mm_store_ps(pOut->max.mData, max);
}

_FORCE_INLINE void ProjectPointsToAABox(float nearPlane, float farPlane, const gmtl::VecA4f* pIn, AABox* pOut)
{
    // Clamp to prevent reverse projection
    __m128 clamper		= _mm_load_ss(&nearPlane);
    __m128 negClamper	= _mm_set_ss(-999999999999.0f);
    negClamper = _mm_movelh_ps(negClamper, clamper);
    clamper = _mm_shuffle_ps(negClamper, negClamper, _MM_SHUFFLE(2, 0, 0, 0));

    __m128 transformedCorner = _mm_load_ps((float*) pIn);
    __m128 dx = _mm_load_ps((float*) &(pIn[1]));
    __m128 dy = _mm_load_ps((float*) &(pIn[2]));
    __m128 dxdy = _mm_load_ps((float*) &(pIn[3]));

    __m128 dz = _mm_load_ps((float*) &(pIn[4]));
    __m128 dxdz = _mm_load_ps((float*) &(pIn[5]));
    __m128 dydz = _mm_load_ps((float*) &(pIn[6]));
    __m128 dxdydz = _mm_load_ps((float*) &(pIn[7]));

    transformedCorner = _mm_max_ps(clamper, transformedCorner);
    dx = _mm_max_ps(clamper, dx);
    dy = _mm_max_ps(clamper, dy);
    dxdy = _mm_max_ps(clamper, dxdy);

    dz = _mm_max_ps(clamper, dz);
    dxdz = _mm_max_ps(clamper, dxdz);
    dydz = _mm_max_ps(clamper, dydz);
    dxdydz = _mm_max_ps(clamper, dxdydz);

    // Do projection with linear Z
    __m128 tempUnit = _mm_set_ss(farPlane);
    __m128 wXmm = _mm_shuffle_ps(transformedCorner, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    transformedCorner = _mm_mul_ps(transformedCorner, wXmm);

    wXmm = _mm_shuffle_ps(dx, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dx = _mm_mul_ps(dx, wXmm);

    wXmm = _mm_shuffle_ps(dy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dy = _mm_mul_ps(dy, wXmm);

    wXmm = _mm_shuffle_ps(dxdy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dxdy = _mm_mul_ps(dxdy, wXmm);

    wXmm = _mm_shuffle_ps(dz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dz = _mm_mul_ps(dz, wXmm);

    wXmm = _mm_shuffle_ps(dxdz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dxdz = _mm_mul_ps(dxdz, wXmm);

    wXmm = _mm_shuffle_ps(dydz, tempUnit, _MM_SHUFFLE(0, 0,  3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dydz = _mm_mul_ps(dydz, wXmm);

    wXmm = _mm_shuffle_ps(dxdydz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    dxdydz = _mm_mul_ps(dxdydz, wXmm);

    // Now find the AABox
    __m128 min = _mm_min_ps(transformedCorner, dx);
    __m128 max = _mm_max_ps(transformedCorner, dx);
    min = _mm_min_ps(min, dy);
    max = _mm_max_ps(max, dy);
    min = _mm_min_ps(min, dxdy);
    max = _mm_max_ps(max, dxdy);
    min = _mm_min_ps(min, dz);
    max = _mm_max_ps(max, dz);
    min = _mm_min_ps(min, dxdz);
    max = _mm_max_ps(max, dxdz);
    min = _mm_min_ps(min, dydz);
    max = _mm_max_ps(max, dydz);
    min = _mm_min_ps(min, dxdydz);
    max = _mm_max_ps(max, dxdydz);

    _mm_store_ps(pOut->min.mData, min);	
    _mm_store_ps(pOut->max.mData, max);
}

_FORCE_INLINE void
TransformAndProjectOOBoxToAABox(const gmtl::MatrixA44f* pMat, float nearPlane, float farPlane, const OOBox* pIn, AABox* pOut)
{
	_TransformAndProjectOOBoxToAABox((__m128*)pMat, nearPlane, farPlane, pIn, pOut);
}

_FORCE_INLINE void
_TransformAndProjectOOBoxToAABox(__m128* pMat, float nearPlane, float farPlane, const OOBox* pIn, AABox* pOut)
{
    __m128 points[8];
    _TransformOOBoxToPoints(pMat, pIn, points);

    __m128 transformedCorner = points[0];
    __m128 dx = points[1]; // dx
    __m128 dy = points[2]; // dy
    __m128 dxdy = points[3]; // dx + dy

    __m128 dz = points[4]; // dz
    __m128 dxdz = points[5]; // dx + dz
    __m128 dydz = points[6]; // dy + dz
    __m128 dxdydz = points[7]; // dx + dy + dz

	// Clamp to prevent reverse projection
	__m128 clamper		= _mm_load_ss(&nearPlane);
	__m128 negClamper	= _mm_set_ss(-999999999999.0f);
	negClamper = _mm_movelh_ps(negClamper, clamper);
	clamper = _mm_shuffle_ps(negClamper, negClamper, _MM_SHUFFLE(2, 0, 0, 0));
		
	transformedCorner = _mm_max_ps(clamper, transformedCorner);
	dx = _mm_max_ps(clamper, dx);
	dy = _mm_max_ps(clamper, dy);
	dxdy = _mm_max_ps(clamper, dxdy);

	dz = _mm_max_ps(clamper, dz);
	dxdz = _mm_max_ps(clamper, dxdz);
	dydz = _mm_max_ps(clamper, dydz);
	dxdydz = _mm_max_ps(clamper, dxdydz);

	// Do projection with linear Z
	__m128 tempUnit = _mm_set_ss(farPlane);
	__m128 wXmm = _mm_shuffle_ps(transformedCorner, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	transformedCorner = _mm_mul_ps(transformedCorner, wXmm);
	
	wXmm = _mm_shuffle_ps(dx, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dx = _mm_mul_ps(dx, wXmm);

	wXmm = _mm_shuffle_ps(dy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dy = _mm_mul_ps(dy, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdy = _mm_mul_ps(dxdy, wXmm);
	
	wXmm = _mm_shuffle_ps(dz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dz = _mm_mul_ps(dz, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdz = _mm_mul_ps(dxdz, wXmm);
	
	wXmm = _mm_shuffle_ps(dydz, tempUnit, _MM_SHUFFLE(0, 0,  3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dydz = _mm_mul_ps(dydz, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdydz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdydz = _mm_mul_ps(dxdydz, wXmm);
	
	// Now find the AABox
	__m128 min = _mm_min_ps(transformedCorner, dx);
	__m128 max = _mm_max_ps(transformedCorner, dx);
	min = _mm_min_ps(min, dy);
	max = _mm_max_ps(max, dy);
	min = _mm_min_ps(min, dxdy);
	max = _mm_max_ps(max, dxdy);
	min = _mm_min_ps(min, dz);
	max = _mm_max_ps(max, dz);
	min = _mm_min_ps(min, dxdz);
	max = _mm_max_ps(max, dxdz);
	min = _mm_min_ps(min, dydz);
	max = _mm_max_ps(max, dydz);
	min = _mm_min_ps(min, dxdydz);
	max = _mm_max_ps(max, dxdydz);

	_mm_store_ps(pOut->min.mData, min);	
	_mm_store_ps(pOut->max.mData, max);
}

_FORCE_INLINE void
TransformAABoxToAABox(const gmtl::MatrixA44f* pMat, const AABox* pIn, AABox* pOut)
{
	_TransformAABoxToAABox((__m128*) pMat, pIn, pOut);
}

_FORCE_INLINE void
_TransformAABoxToAABox(__m128* pMat, const AABox* pIn, AABox* pOut)
{
    __m128 points[8];
    _TransformAABoxToPoints(pMat, pIn, points);

    __m128 transformedCorner = points[0];
    __m128 dx = points[1]; // dx
    __m128 dy = points[2]; // dy
    __m128 dxdy = points[3]; // dx + dy

    __m128 dz = points[4]; // dz
    __m128 dxdz = points[5]; // dx + dz
    __m128 dydz = points[6]; // dy + dz
    __m128 dxdydz = points[7]; // dx + dy + dz
	
	// Now find the AABox
	__m128 min = _mm_min_ps(transformedCorner, dx);
	__m128 max = _mm_max_ps(transformedCorner, dx);
	min = _mm_min_ps(min, dy);
	max = _mm_max_ps(max, dy);
	min = _mm_min_ps(min, dxdy);
	max = _mm_max_ps(max, dxdy);
	min = _mm_min_ps(min, dz);
	max = _mm_max_ps(max, dz);
	min = _mm_min_ps(min, dxdz);
	max = _mm_max_ps(max, dxdz);
	min = _mm_min_ps(min, dydz);
	max = _mm_max_ps(max, dydz);
	min = _mm_min_ps(min, dxdydz);
	max = _mm_max_ps(max, dxdydz);

	//__m128 aabbMaxExtentsAbs    = _mm_add_ps(aabbExtentsPostX, aabbExtentsPostY);
	//aabbMaxExtentsAbs           = _mm_add_ps(aabbMaxExtentsAbs, aabbExtentsPostZ);

	//__m128 maxPost  = _mm_add_ps(aabbCenterPost.Vec128(), aabbMaxExtentsAbs);
	//__m128 minPost  = _mm_sub_ps(aabbCenterPost.Vec128(), aabbMaxExtentsAbs);

	_mm_store_ps(pOut->min.mData, min);	
	_mm_store_ps(pOut->max.mData, max);
}

_FORCE_INLINE void
TransformAndProjectAABoxToAABox(const gmtl::MatrixA44f* pMat, float nearPlane, float farPlane, const AABox* pIn, AABox* pOut)
{
	_TransformAndProjectAABoxToAABox((__m128*)pMat, nearPlane, farPlane, pIn, pOut);
}

_FORCE_INLINE void
_TransformAndProjectAABoxToAABox(__m128* pMat, float nearPlane, float farPlane, const AABox* pIn, AABox* pOut)
{
    __m128 points[8];
    _TransformAABoxToPoints(pMat, pIn, points);

    __m128 transformedCorner = points[0];
    __m128 dx = points[1]; // dx
    __m128 dy = points[2]; // dy
    __m128 dxdy = points[3]; // dx + dy

    __m128 dz = points[4]; // dz
    __m128 dxdz = points[5]; // dx + dz
    __m128 dydz = points[6]; // dy + dz
    __m128 dxdydz = points[7]; // dx + dy + dz
	
	// Clamp to prevent reverse projection
	__m128 clamper		= _mm_load_ss(&nearPlane);
	__m128 negClamper	= _mm_set_ss(-999999999999.0f);
	negClamper = _mm_movelh_ps(negClamper, clamper);
	clamper = _mm_shuffle_ps(negClamper, negClamper, _MM_SHUFFLE(2, 0, 0, 0));
	
	transformedCorner = _mm_max_ps(clamper, transformedCorner);
	dx = _mm_max_ps(clamper, dx);
	dy = _mm_max_ps(clamper, dy);
	dxdy = _mm_max_ps(clamper, dxdy);

	dz = _mm_max_ps(clamper, dz);
	dxdz = _mm_max_ps(clamper, dxdz);
	dydz = _mm_max_ps(clamper, dydz);
	dxdydz = _mm_max_ps(clamper, dxdydz);

	// Do projection with linear Z
	__m128 tempUnit = _mm_set_ss(farPlane);
	__m128 wXmm = _mm_shuffle_ps(transformedCorner, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	transformedCorner = _mm_mul_ps(transformedCorner, wXmm);
	
	wXmm = _mm_shuffle_ps(dx, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); // r3=farZ r2=farZ r1=w r0=w
	wXmm = _mm_rcp_ps(wXmm);
	dx = _mm_mul_ps(dx, wXmm);

	wXmm = _mm_shuffle_ps(dy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dy = _mm_mul_ps(dy, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdy, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdy = _mm_mul_ps(dxdy, wXmm);
	
	wXmm = _mm_shuffle_ps(dz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dz = _mm_mul_ps(dz, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdz = _mm_mul_ps(dxdz, wXmm);
	
	wXmm = _mm_shuffle_ps(dydz, tempUnit, _MM_SHUFFLE(0, 0,  3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dydz = _mm_mul_ps(dydz, wXmm);
	
	wXmm = _mm_shuffle_ps(dxdydz, tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
	wXmm = _mm_rcp_ps(wXmm);
	dxdydz = _mm_mul_ps(dxdydz, wXmm);
	
	// Now find the AABox
	__m128 min = _mm_min_ps(transformedCorner, dx);
	__m128 max = _mm_max_ps(transformedCorner, dx);
	min = _mm_min_ps(min, dy);
	max = _mm_max_ps(max, dy);
	min = _mm_min_ps(min, dxdy);
	max = _mm_max_ps(max, dxdy);
	min = _mm_min_ps(min, dz);
	max = _mm_max_ps(max, dz);
	min = _mm_min_ps(min, dxdz);
	max = _mm_max_ps(max, dxdz);
	min = _mm_min_ps(min, dydz);
	max = _mm_max_ps(max, dydz);
	min = _mm_min_ps(min, dxdydz);
	max = _mm_max_ps(max, dxdydz);

	_mm_store_ps(pOut->min.mData, min);	
	_mm_store_ps(pOut->max.mData, max);
}

_FORCE_INLINE void _TransformOOBoxToPoints(__m128* pMat, const OOBox* pIn, __m128* pOut)
{
	//// Load the transform matrix	
	//col0 = pMat[0];
	//col1 = pMat[1];
	//col2 = pMat[2];
	//trans = pMat[3];

	// Load the corner data
	__m128& cornerXmm = *((__m128*)(pIn->corner.mData));

	// Broadcast the corner x, y and z	
	__m128 cornerX = _mm_shuffle_ps(cornerXmm, cornerXmm, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 cornerZ = _mm_shuffle_ps(cornerXmm, cornerXmm, _MM_SHUFFLE(2, 2, 2, 2));
	__m128 cornerY = _mm_shuffle_ps(cornerXmm, cornerXmm, _MM_SHUFFLE(1, 1, 1, 1));

	// Transform the corner
	cornerX = _mm_mul_ps(cornerX, pMat[0]);
	cornerY = _mm_mul_ps(cornerY, pMat[1]);
	cornerZ = _mm_mul_ps(cornerZ, pMat[2]);

	__m128 temp1 = _mm_add_ps(cornerX, cornerY);
	__m128 temp2 = _mm_add_ps(cornerZ, pMat[3]);
	__m128 transformedCorner = _mm_add_ps(temp1, temp2);

	// X, Y and Z-axis
	__m128& extentsX = *((__m128*)(pIn->xExtents.mData));
	__m128& extentsY = *((__m128*)(pIn->yExtents.mData));
	__m128& extentsZ = *((__m128*)(pIn->zExtents.mData));

	__m128 extentXx = _mm_shuffle_ps(extentsX, extentsX, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 extentYx = _mm_shuffle_ps(extentsY, extentsY, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 extentZx = _mm_shuffle_ps(extentsZ, extentsZ, _MM_SHUFFLE(0, 0, 0, 0));
	__m128 extentXy = _mm_shuffle_ps(extentsX, extentsX, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 extentYy = _mm_shuffle_ps(extentsY, extentsY, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 extentZy = _mm_shuffle_ps(extentsZ, extentsZ, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 extentXz = _mm_shuffle_ps(extentsX, extentsX, _MM_SHUFFLE(2, 2, 2, 2));
	__m128 extentYz = _mm_shuffle_ps(extentsY, extentsY, _MM_SHUFFLE(2, 2, 2, 2));
	__m128 extentZz = _mm_shuffle_ps(extentsZ, extentsZ, _MM_SHUFFLE(2, 2, 2, 2));
	
	extentXx = _mm_mul_ps(extentXx, pMat[0]);
	extentYx = _mm_mul_ps(extentYx, pMat[0]);
	extentZx = _mm_mul_ps(extentZx, pMat[0]);
	extentXy = _mm_mul_ps(extentXy, pMat[1]);
	extentYy = _mm_mul_ps(extentYy, pMat[1]);
	extentZy = _mm_mul_ps(extentZy, pMat[1]);
	extentXz = _mm_mul_ps(extentXz, pMat[2]);
	extentYz = _mm_mul_ps(extentYz, pMat[2]);
	extentZz = _mm_mul_ps(extentZz, pMat[2]);

	__m128 transformedX = _mm_add_ps(extentXx, extentXy);
	transformedX = _mm_add_ps(transformedX, extentXz);
	__m128 transformedY = _mm_add_ps(extentYx, extentYy);
	transformedY = _mm_add_ps(transformedY, extentYz);
	__m128 transformedZ = _mm_add_ps(extentZx, extentZy);
	transformedZ = _mm_add_ps(transformedZ, extentZz);

	// Now we get the eight points of the OBB
	__m128 dx = _mm_add_ps(transformedCorner, transformedX);	// dx
	__m128 dy = _mm_add_ps(transformedCorner, transformedY);	// dy
	__m128 dxdy = _mm_add_ps(dx, transformedY);	// dx + dy

	__m128 dz = _mm_add_ps(transformedCorner, transformedZ);	// dz
	__m128 dxdz = _mm_add_ps(dx, transformedZ);	// dx + dz
	__m128 dydz = _mm_add_ps(dy, transformedZ);	// dy + dz
	__m128 dxdydz = _mm_add_ps(dxdy, transformedZ);	// dx + dy + dz
	
    pOut[0] = transformedCorner;
    pOut[1] = dx;
    pOut[2] = dy;
    pOut[3] = dxdy;

    pOut[4] = dz;
    pOut[5] = dxdz;
    pOut[6] = dydz;
    pOut[7] = dxdydz;
}

_FORCE_INLINE void _TransformAABoxToPoints(__m128* pMat, const AABox* pIn, __m128* pOut)
{
    __m128& aaMax = *((__m128*)(pIn->max.mData));
    __m128& aaMin = *((__m128*)(pIn->min.mData));

    // Get the extents 
    __m128 extents = _mm_sub_ps(aaMax, aaMin);

    // Broadcast the corner point
    __m128 cornerY = _mm_shuffle_ps(aaMin, aaMin, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 cornerZ = _mm_shuffle_ps(aaMin, aaMin, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 cornerX = _mm_shuffle_ps(aaMin, aaMin, _MM_SHUFFLE(0, 0, 0, 0));

    // Transform the corner point
    cornerX						= _mm_mul_ps(cornerX, pMat[0]);
    cornerY						= _mm_mul_ps(cornerY, pMat[1]);
    cornerZ						= _mm_mul_ps(cornerZ, pMat[2]);
    cornerX						= _mm_add_ps(cornerX, cornerZ);
    cornerY						= _mm_add_ps(cornerY, pMat[3]);
    __m128 transformedCorner	= _mm_add_ps(cornerX, cornerY);

    // Broadcast the extents
    __m128 extentX	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 extentZ	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 extentY	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(1, 1, 1, 1));

    // Transform the extents
    __m128 transformedX = _mm_mul_ps(extentX, pMat[0]);
    __m128 transformedY = _mm_mul_ps(extentY, pMat[1]);
    __m128 transformedZ = _mm_mul_ps(extentZ, pMat[2]);

    // Now we get the eight points of the OBB
    __m128 dx = _mm_add_ps(transformedCorner, transformedX);	// dx
    __m128 dy = _mm_add_ps(transformedCorner, transformedY);	// dy
    __m128 dxdy = _mm_add_ps(dx, transformedY);	// dx + dy

    __m128 dz = _mm_add_ps(transformedCorner, transformedZ);	// dz
    __m128 dxdz = _mm_add_ps(dx, transformedZ);	// dx + dz
    __m128 dydz = _mm_add_ps(dy, transformedZ);	// dy + dz
    __m128 dxdydz = _mm_add_ps(dxdy, transformedZ);	// dx + dy + dz

    pOut[0] = transformedCorner;
    pOut[1] = dx;
    pOut[2] = dy;
    pOut[3] = dxdy;
    pOut[4] = dz;
    pOut[5] = dxdz;
    pOut[6] = dydz;
    pOut[7] = dxdydz;
}

_FORCE_INLINE void
TransformOOBoxToPoints(const gmtl::MatrixA44f* pMat, const OOBox* pIn, gmtl::VecA4f* pOut)
{
//	__m128 mat[4];
	//LoadMatrix44(pMat, mat);
	_TransformOOBoxToPoints((__m128*) pMat, pIn, (__m128*) pOut);
}

//inline void
//_TransformAndProjectOOBoxToAABox(__m128 pMat[4], const OOBox* pIn, AABox* pOut)
//{
//	__m128 xmm0, xmm1, xmm2, xmm3;
//	__m128 col0, col1, col2, trans;
//
//	// Load the transform matrix		
//	col0 = pMat[0];
//	col1 = pMat[1];
//	col2 = pMat[2];
//	trans = pMat[3];
//
//	// Load the corner data
//	xmm1 = _mm_load_ps(pIn->corner.mData);
//
//	// Broadcast the corner x, y and z	
//	xmm0 = _mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(0, 0, 0, 0));
//	xmm2 = _mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(2, 2, 2, 2));
//	xmm1 = _mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(1, 1, 1, 1));
//
//	// Transform the corner
//	xmm0 = _mm_mul_ps(xmm0, col0);
//	xmm1 = _mm_mul_ps(xmm1, col1);
//	xmm2 = _mm_mul_ps(xmm2, col2);
//
//	xmm0 = _mm_add_ps(xmm0, xmm1);
//	xmm2 = _mm_add_ps(xmm2, trans);
//	xmm0 = _mm_add_ps(xmm0, xmm2);
//
//	// X and Y-axis
//	// Process x
//	xmm1 = _mm_load_ps1(pIn->xExtents.mData);
//	xmm2 = _mm_load_ps1(pIn->yExtents.mData);
//	xmm3 = _mm_mul_ps(xmm1, col0);
//	trans = _mm_mul_ps(xmm2, col0);
//
//	// Process y
//	xmm1 = _mm_load_ps1(pIn->xExtents.mData+1);
//	xmm2 = _mm_load_ps1(pIn->yExtents.mData+1);
//	xmm1 = _mm_mul_ps(xmm1, col1);
//	xmm2 = _mm_mul_ps(xmm2, col1);
//	xmm3 = _mm_add_ps(xmm3, xmm1);
//	trans = _mm_add_ps(trans, xmm2);
//
//	// Process z
//	xmm1 = _mm_load_ps1(pIn->xExtents.mData+2);
//	xmm2 = _mm_load_ps1(pIn->yExtents.mData+2);
//	xmm1 = _mm_mul_ps(xmm1, col2);
//	xmm2 = _mm_mul_ps(xmm2, col2);
//	xmm1 = _mm_add_ps(xmm3, xmm1);
//	xmm2 = _mm_add_ps(trans, xmm2);
//
//	// Z-axis
//	// Process x
//	xmm3 = _mm_load_ps(pIn->zExtents.mData);
//	trans = _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(0, 0, 0, 0));
//	col0 = _mm_mul_ps(trans, col0);
//
//	// Process y and z
//	trans = _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(1, 1, 1, 1));
//	xmm3 = _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(2, 2, 2, 2));
//	trans = _mm_mul_ps(trans, col1);
//	xmm3 = _mm_mul_ps(xmm3, col2);
//	col0 = _mm_add_ps(col0, trans);
//	xmm3 = _mm_add_ps(col0, xmm3);
//
//	// Now we get the eight points of the OBB and do projection to 3D
//	// and find the min and max
//
//	col0 = _mm_add_ps(xmm0, xmm1);	// dx
//	col1 = _mm_add_ps(xmm0, xmm2);	// dy
//	col2 = _mm_add_ps(col0, xmm2);	// dx + dy
//
//	trans = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	xmm1 = _mm_mul_ps(trans, xmm0);		
//
//	trans = _mm_shuffle_ps(col0, col0, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col0);
//	xmm2 = _mm_max_ps(xmm1, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	trans = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col1);
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	trans = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col2);
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	xmm0 = _mm_add_ps(xmm0, xmm3);	// dz
//	col0 = _mm_add_ps(col0, xmm3);	// dx + dz
//	col1 = _mm_add_ps(col1, xmm3);	// dy + dz
//	col2 = _mm_add_ps(col2, xmm3);	// dx + dy + dz
//
//	trans = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, xmm0);		
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	trans = _mm_shuffle_ps(col0, col0, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col0);
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	trans = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col1);
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	trans = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(3, 3, 3, 3));
//	trans = _mm_rcp_ps(trans);
//	trans = _mm_mul_ps(trans, col2);
//	xmm2 = _mm_max_ps(xmm2, trans);
//	xmm1 = _mm_min_ps(xmm1, trans);
//
//	_mm_store_ps(&(pOut->min), xmm1);	
//	_mm_store_ps(&(pOut->max), xmm2);
//}

//inline void
//TransformAndProjectOOBoxToAABox(const gmtl::Matrix44f* pMat, const OOBox* pIn, AABox* pOut)
//{
//	__m128 mat[4];
//	LoadMatrix44(pMat, mat);
//	_TransformAndProjectOOBoxToAABox(mat, pIn, pOut);
//}

_FORCE_INLINE void
OOBoxToPoints(const OOBox* pIn, gmtl::VecA3f* pOut)
{
	__m128 xmm0;
	__m128 col0, col1, col2;

	__m128& cornerXmm = *((__m128*)(pIn->corner.mData));
	__m128& xXmm = *((__m128*)(pIn->xExtents.mData));
	__m128& yXmm = *((__m128*)(pIn->yExtents.mData));
	__m128& zXmm = *((__m128*)(pIn->zExtents.mData));

	//xmm0 = _mm_load_ps(pIn->corner.mData);
	//xmm1 = _mm_load_ps(pIn->xExtents.mData);
	//xmm2 = _mm_load_ps(pIn->yExtents.mData);
	//xmm3 = _mm_load_ps(pIn->zExtents.mData);

	col0 = _mm_add_ps(cornerXmm, xXmm);
	col1 = _mm_add_ps(cornerXmm, yXmm);
	col2 = _mm_add_ps(col0, yXmm);
	_mm_store_ps(pOut->mData, cornerXmm);
	_mm_store_ps((pOut+1)->mData, col0);
	_mm_store_ps((pOut+2)->mData, col1);
	_mm_store_ps((pOut+3)->mData, col2);

	xmm0 = _mm_add_ps(cornerXmm, zXmm);
	col0 = _mm_add_ps(col0, zXmm);
	col1 = _mm_add_ps(col1, zXmm);
	col2 = _mm_add_ps(col2, zXmm);
	_mm_store_ps((pOut+4)->mData, xmm0);
	_mm_store_ps((pOut+5)->mData, col0);
	_mm_store_ps((pOut+6)->mData, col1);
	_mm_store_ps((pOut+7)->mData, col2);
}

_FORCE_INLINE void
OOBoxToAABox(const OOBox* pIn, AABox* pOut)
{
	__m128 xmm0, xmm1;
	__m128 col0, col1, col2, col3;

	__m128& cornerXmm = *((__m128*)(pIn->corner.mData));
	__m128& xXmm = *((__m128*)(pIn->xExtents.mData));
	__m128& yXmm = *((__m128*)(pIn->yExtents.mData));
	__m128& zXmm = *((__m128*)(pIn->zExtents.mData));
	
	//xmm0 = _mm_load_ps(pIn->corner.mData);
	//xmm1 = _mm_load_ps(pIn->xExtents.mData);
	//xmm2 = _mm_load_ps(pIn->yExtents.mData);
	//xmm3 = _mm_load_ps(pIn->zExtents.mData);

	// Get the 4 top corners
	col0 = _mm_add_ps(cornerXmm, xXmm);
	col1 = _mm_add_ps(cornerXmm, yXmm);
	col2 = _mm_add_ps(col0, yXmm);
	
	// Find the min and max of the 4 top corners	
	col3 = _mm_min_ps(cornerXmm, col0);
	xmm1 = _mm_max_ps(cornerXmm, col0);
	
	col3 = _mm_min_ps(col1, col3);
	xmm1 = _mm_max_ps(col1, xmm1);
	
	col3 = _mm_min_ps(col2, col3);
	xmm1 = _mm_max_ps(col2, xmm1);	

	// Get the 4 bottom corners
	xmm0 = _mm_add_ps(cornerXmm, zXmm);
	col0 = _mm_add_ps(col0, zXmm);
	col1 = _mm_add_ps(col1, zXmm);
	col2 = _mm_add_ps(col2, zXmm);
	
	// Find the min and max of the 4 bottom corners
	col3 = _mm_min_ps(xmm0, col3);
	xmm1 = _mm_max_ps(xmm0, xmm1);

	col3 = _mm_min_ps(col0, col3);
	xmm1 = _mm_max_ps(col0, xmm1);

	col3 = _mm_min_ps(col1, col3);
	xmm1 = _mm_max_ps(col1, xmm1);

	col3 = _mm_min_ps(col2, col3);
	xmm1 = _mm_max_ps(col2, xmm1);
	
	_mm_store_ps(pOut->min.mData, col3);
	_mm_store_ps(pOut->max.mData, xmm1);
}

_FORCE_INLINE void
OOBoxPointsToAABox(const gmtl::VecA3f* pIn, AABox* pOut)
{
	__m128 xmm6, xmm7;
	
	__m128& p1 = *((__m128*)(pIn->mData));
	__m128& p2 = *((__m128*)((pIn+1)->mData));
	__m128& p3 = *((__m128*)((pIn+2)->mData));
	__m128& p4 = *((__m128*)((pIn+3)->mData));

	xmm6 = _mm_min_ps(p1, p2);
	xmm7 = _mm_max_ps(p1, p2);
	xmm6 = _mm_min_ps(xmm6, p3);
	xmm7 = _mm_max_ps(xmm7, p3);
	xmm6 = _mm_min_ps(xmm6, p4);
	xmm7 = _mm_max_ps(xmm7, p4);

	__m128& p5 = *((__m128*)((pIn+4)->mData));
	__m128& p6 = *((__m128*)((pIn+5)->mData));
	__m128& p7 = *((__m128*)((pIn+6)->mData));
	__m128& p8 = *((__m128*)((pIn+7)->mData));

	xmm6 = _mm_min_ps(xmm6, p5);
	xmm7 = _mm_max_ps(xmm7, p5);
	xmm6 = _mm_min_ps(xmm6, p6);
	xmm7 = _mm_max_ps(xmm7, p6);
	xmm6 = _mm_min_ps(xmm6, p7);
	xmm7 = _mm_max_ps(xmm7, p7);
	xmm6 = _mm_min_ps(xmm6, p8);
	xmm7 = _mm_max_ps(xmm7, p8);
	
	_mm_store_ps((float*) &(pOut->min), xmm6);
	_mm_store_ps((float*) &(pOut->max), xmm7);
}

_FORCE_INLINE void
PointsToAABox(const gmtl::VecA3f* pIn, int length, AABox* pOut)
{
	__m128 xmm0, xmm1, xmm2, xmm3;
	
	if(length & 0x1)
	{
		xmm0 = _mm_load_ps((float*) pIn);
		xmm1 = xmm0;
	
		length >>= 1;
		pIn++;
		
		_LOOPi(length)
		{
			xmm2 = _mm_load_ps((float*) pIn);
			xmm3 = _mm_load_ps((float*) (pIn+1));
			xmm0 = _mm_min_ps(xmm0, xmm2);
			xmm1 = _mm_max_ps(xmm1, xmm2);
			xmm0 = _mm_min_ps(xmm0, xmm3);
			xmm1 = _mm_max_ps(xmm1, xmm3);			
			pIn+=2;
		}
	}
	else
	{
		xmm2 = _mm_load_ps((float*) pIn);
		xmm3 = _mm_load_ps((float*) (pIn+1));
		
		xmm0 = _mm_min_ps(xmm2, xmm3);
		xmm1 = _mm_max_ps(xmm2, xmm3);
	
		length -= 2;
		length >>= 1;		
		pIn+=2;
		
		_LOOPi(length)
		{
			xmm2 = _mm_load_ps((float*) pIn);
			xmm3 = _mm_load_ps((float*) (pIn+1));
			xmm0 = _mm_min_ps(xmm0, xmm2);
			xmm1 = _mm_max_ps(xmm1, xmm2);
			xmm0 = _mm_min_ps(xmm0, xmm3);
			xmm1 = _mm_max_ps(xmm1, xmm3);
			pIn+=2;
			
		}
	}
	
	_mm_store_ps((float*) &(pOut->min), xmm0);
	_mm_store_ps((float*) &(pOut->max), xmm1);	
}

_FORCE_INLINE void
PointsToAABox(const gmtl::Vec4f* pIn, int length, int stride, AABox* pOut)
{
	__m128 xmm0, xmm1, xmm2, xmm3;

	if(length & 0x1)
	{
		xmm0 = _mm_loadu_ps((float*) pIn);
		xmm1 = xmm0;

		length >>= 1;
		pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);

		_LOOPi(length)
		{
			xmm2 = _mm_loadu_ps((float*) pIn);
			pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);
			xmm3 = _mm_loadu_ps((float*) pIn);
			
			xmm0 = _mm_min_ps(xmm0, xmm2);
			xmm1 = _mm_max_ps(xmm1, xmm2);
			xmm0 = _mm_min_ps(xmm0, xmm3);
			xmm1 = _mm_max_ps(xmm1, xmm3);			
			pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);
		}
	}
	else
	{
		xmm2 = _mm_loadu_ps((float*) pIn);
		pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);
		xmm3 = _mm_loadu_ps((float*) pIn);

		xmm0 = _mm_min_ps(xmm2, xmm3);
		xmm1 = _mm_max_ps(xmm2, xmm3);

		length -= 2;
		length >>= 1;		
		pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);

		_LOOPi(length)
		{
			xmm2 = _mm_loadu_ps((float*) pIn);
			pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);
			xmm3 = _mm_loadu_ps((float*) pIn);
			
			xmm0 = _mm_min_ps(xmm0, xmm2);
			xmm1 = _mm_max_ps(xmm1, xmm2);
			xmm0 = _mm_min_ps(xmm0, xmm3);
			xmm1 = _mm_max_ps(xmm1, xmm3);
			
			pIn = (const gmtl::Vec4f*) (((byte*) pIn) + stride);
		}
	}

	_mm_store_ps((float*) &(pOut->min), xmm0);
	_mm_store_ps((float*) &(pOut->max), xmm1);	
}


_FORCE_INLINE boolean
AABoxInAABox(const AABox* pBox1, const AABox* pBox2)
{
	return _AABoxInAABox(pBox1, pBox2);
}

_FORCE_INLINE boolean
_AABoxInAABox(const AABox* pBox1, const AABox* pBox2)
{
	__m128 isLessThan = _mm_cmplt_ps(*(__m128*) pBox1->min.mData, *(__m128*) pBox2->max.mData);
	__m128 isGreaterThan = _mm_cmpgt_ps(*(__m128*) pBox1->max.mData, *(__m128*) pBox2->min.mData);
	
	__m128 res = _mm_and_ps(isLessThan, isGreaterThan);
	int val = _mm_movemask_ps(res);
	return ((val & 0x7) == 0x7);
}

_FORCE_INLINE boolean
AABoxContainsAABox(const AABox* pContainer, const AABox* pContainee)
{
	__m128 isLessThan = _mm_cmplt_ps(*(__m128*) pContainee->max.mData, *(__m128*) pContainer->max.mData);
	__m128 isGreaterThan = _mm_cmpgt_ps(*(__m128*) pContainee->min.mData, *(__m128*) pContainer->min.mData);
	
	__m128 res = _mm_and_ps(isLessThan, isGreaterThan);
	int val = _mm_movemask_ps(res);
	return ((val & 0x7) == 0x7);
}

_FORCE_INLINE void
AABoxMerge(AABox* pResult, const AABox* pBox1, const AABox* pBox2)
{
	__m128 max = _mm_max_ps(*(__m128*) pBox1->max.mData, *(__m128*) pBox2->max.mData);
	__m128 min = _mm_min_ps(*(__m128*) pBox1->min.mData, *(__m128*) pBox2->min.mData);

	_mm_store_ps(pResult->max.mData, max);
	_mm_store_ps(pResult->min.mData, min);
}

_FORCE_INLINE boolean
AABoxInSphere(const AABox* pBox, const Sphere* pSphere)
{
	const static _ALIGN(16) float ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	

	__m128& center	= *((__m128*)(pSphere->center.mData));
	__m128& boxMin	= *((__m128*)(pBox->min.mData));
	__m128& boxMax	= *((__m128*)(pBox->max.mData));

	__m128 centerToMin	= _mm_sub_ps(boxMin, center);
	centerToMin			= _mm_max_ps(centerToMin, *((__m128*)ZERO));

	__m128 maxToCenter	= _mm_sub_ps(center, boxMax);
	maxToCenter			= _mm_max_ps(maxToCenter, *((__m128*)ZERO));

	__m128 dist			= _mm_add_ps(centerToMin , maxToCenter);

	//__m128 isLessThan			= _mm_cmplt_ps(center, boxMin);
	//__m128 isGreaterThan		= _mm_cmpgt_ps(center, boxMax);
	////__m128 multIsLessThan		= _mm_and_ps(isLessThan, *((__m128*)multiplier));
	////__m128 multIsGreaterThan	= _mm_and_ps(isGreaterThan, *((__m128*)multiplier));
	//
	//__m128 distFromMin	= _mm_sub_ps(center, boxMin);
	//__m128 distFromMax	= _mm_sub_ps(center, boxMax);
	////distFromMin			= _mm_mul_ps(distFromMin, multIsLessThan);
	////distFromMax			= _mm_mul_ps(distFromMax, multIsGreaterThan);
	//distFromMin			= _mm_and_ps(distFromMin, isLessThan);
	//distFromMax			= _mm_and_ps(distFromMax, isGreaterThan);
	//__m128 dist			= _mm_add_ps(distFromMin, distFromMax);
	//dist				= _mm_mul_ps(dist, dist);
	
	// Collapse to get the total distance
	// SSE 4.1
	dist				= _mm_dp_ps(dist, dist, 0x7F);

	//dist				= _mm_mul_ps(dist, dist);
	//__m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
	//collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;
	
	__m128 radius		= _mm_load_ss(&(pSphere->center[3]));
	radius				= _mm_mul_ss(radius, radius);
	dist				= _mm_cmple_ss(dist, radius);
	
    int isIntersect = _mm_movemask_ps(dist);
    return (isIntersect & 1);
}

_FORCE_INLINE boolean SphereInAABox(const Sphere* pSphere, const AABox* pBox)
{
	return AABoxInSphere(pBox, pSphere);
}

_FORCE_INLINE boolean SphereContainsAABox(const Sphere* pSphere, const AABox* pBox)
{
	__m128& center	= *((__m128*)(pSphere->center.mData));
	__m128& boxMin	= *((__m128*)(pBox->min.mData));
	__m128& boxMax	= *((__m128*)(pBox->max.mData));

	// Find the farthest point of the AABB from the center of the sphere
	__m128 boxCenter			= _mm_mul_ps(_mm_add_ps(boxMax, boxMin), _mm_set1_ps(0.5f));
	__m128 isLessThanEqCenter	= _mm_cmple_ps(center, boxCenter);
	__m128 isGreaterThanCenter	= _mm_cmpgt_ps(center, boxCenter);
	__m128 maxDistPoint			= _mm_and_ps(boxMax, isLessThanEqCenter);
	__m128 minDistPoint			= _mm_and_ps(boxMin, isGreaterThanCenter);
	__m128 farthestPoint		= _mm_or_ps(maxDistPoint, minDistPoint);
	__m128 dist					= _mm_sub_ps(center, farthestPoint);

	// Compute it's distance
	// SSE 4.1
	dist				= _mm_dp_ps(dist, dist, 0x7F);

	//dist				= _mm_mul_ps(dist, dist);			// r = xSq, ySq, zSq, wSq
	//__m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
	//collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;

	// And check against the radius of the sphere
	__m128 radius		= _mm_load_ss(&(pSphere->center[3]));
	radius				= _mm_mul_ss(radius, radius);
	dist				= _mm_cmple_ss(dist, radius);

    int isContained = _mm_movemask_ps(dist);
    return (isContained & 1);
}

_FORCE_INLINE float AABoxToPointDist(const AABox* pBox, const gmtl::VecA3f* pPoint)
{
	//const static _ALIGN(16) float multiplier[4] = { 1.0f, 1.0f, 1.0f, 0.0f };	

	//__m128& center	= *((__m128*)(pPoint->mData));
	//__m128& boxMin	= *((__m128*)(pBox->min.mData));
	//__m128& boxMax	= *((__m128*)(pBox->max.mData));

	//__m128 isLessThan			= _mm_cmplt_ps(center, boxMin);
	//__m128 isGreaterThan		= _mm_cmpgt_ps(center, boxMax);
	////__m128 multIsLessThan		= _mm_and_ps(isLessThan, *((__m128*)multiplier));
	////__m128 multIsGreaterThan	= _mm_and_ps(isGreaterThan, *((__m128*)multiplier));

	//__m128 distFromMin	= _mm_sub_ps(center, boxMin);
	//__m128 distFromMax	= _mm_sub_ps(center, boxMax);
	//distFromMin			= _mm_and_ps(distFromMin, isLessThan);
	//distFromMax			= _mm_and_ps(distFromMax, isGreaterThan);

	const static _ALIGN(16) float ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	

	__m128& center	= *((__m128*)(pPoint->mData));
	__m128& boxMin	= *((__m128*)(pBox->min.mData));
	__m128& boxMax	= *((__m128*)(pBox->max.mData));

	__m128 centerToMin	= _mm_sub_ps(boxMin, center);
	centerToMin			= _mm_max_ps(centerToMin, *((__m128*)ZERO));

	__m128 maxToCenter	= _mm_sub_ps(center, boxMax);
	maxToCenter			= _mm_max_ps(maxToCenter, *((__m128*)ZERO));

	__m128 dist			= _mm_add_ps(centerToMin , maxToCenter);

	// Collapse to get the total distance
	// SSE 4.1
	dist				= _mm_dp_ps(dist, dist, 0x7F);
	dist				= _mm_rcp_ss( _mm_rsqrt_ss(dist) );

	//dist				= _mm_mul_ps(dist, dist);
	//__m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
	//collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
	//dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;
	//dist				= _mm_sqrt_ss(dist);

	float res;
	_mm_store_ss(&res, dist);

	return res;
}

_FORCE_INLINE float AABoxToPointDistCheckLessThan(const AABox* pBox, const gmtl::VecA3f* pPoint, float distToCheck, boolean& checkResult)
{
    //const static _ALIGN(16) float multiplier[4] = { 1.0f, 1.0f, 1.0f, 0.0f };	

    //__m128& center	= *((__m128*)(pPoint->mData));
    //__m128& boxMin	= *((__m128*)(pBox->min.mData));
    //__m128& boxMax	= *((__m128*)(pBox->max.mData));

    //__m128 isLessThan			= _mm_cmplt_ps(center, boxMin);
    //__m128 isGreaterThan		= _mm_cmpgt_ps(center, boxMax);
    ////__m128 multIsLessThan		= _mm_and_ps(isLessThan, *((__m128*)multiplier));
    ////__m128 multIsGreaterThan	= _mm_and_ps(isGreaterThan, *((__m128*)multiplier));

    //__m128 distFromMin	= _mm_sub_ps(center, boxMin);
    //__m128 distFromMax	= _mm_sub_ps(center, boxMax);
    //distFromMin			= _mm_and_ps(distFromMin, isLessThan);
    //distFromMax			= _mm_and_ps(distFromMax, isGreaterThan);

    const static _ALIGN(16) float ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	

    __m128& center	= *((__m128*)(pPoint->mData));
    __m128& boxMin	= *((__m128*)(pBox->min.mData));
    __m128& boxMax	= *((__m128*)(pBox->max.mData));

    __m128 centerToMin	= _mm_sub_ps(boxMin, center);
    centerToMin			= _mm_max_ps(centerToMin, *((__m128*)ZERO));

    __m128 maxToCenter	= _mm_sub_ps(center, boxMax);
    maxToCenter			= _mm_max_ps(maxToCenter, *((__m128*)ZERO));

    __m128 dist			= _mm_add_ps(centerToMin , maxToCenter);

    // Collapse to get the total distance
    // SSE 4.1
    dist				= _mm_dp_ps(dist, dist, 0x7F);
    dist				= _mm_rcp_ss( _mm_rsqrt_ss(dist) );

    //dist				= _mm_mul_ps(dist, dist);
    //__m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
    //dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
    //collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
    //dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;
    //dist				= _mm_sqrt_ss(dist);

    __m128 lessThan     = _mm_cmplt_ss(dist, _mm_set_ss(distToCheck));
    int isLessThan = _mm_movemask_ps(lessThan);
    checkResult = (boolean) (isLessThan & 1);

    float res;
    _mm_store_ss(&res, dist);
    return res;
}

_FORCE_INLINE void FrustumTestClipSpace(const gmtl::VecA4f* pPoints, int* pInsidePlaneMask)
{
    __m128 xxxx1 = _mm_load_ps((float*) pPoints);
    __m128 yyyy1 = _mm_load_ps((float*) &(pPoints[1]));
    __m128 zzzz1 = _mm_load_ps((float*) &(pPoints[2]));
    __m128 wwww1 = _mm_load_ps((float*) &(pPoints[3]));

    __m128 xxxx2 = _mm_load_ps((float*) &(pPoints[4]));
    __m128 yyyy2 = _mm_load_ps((float*) &(pPoints[5]));
    __m128 zzzz2 = _mm_load_ps((float*) &(pPoints[6]));
    __m128 wwww2 = _mm_load_ps((float*) &(pPoints[7]));

    _MM_TRANSPOSE4_PS(xxxx1, yyyy1, zzzz1, wwww1);
    _MM_TRANSPOSE4_PS(xxxx2, yyyy2, zzzz2, wwww2);

    __m128 wwww1Neg = _mm_sub_ps(_mm_setzero_ps(), wwww1);
    __m128 wwww2Neg = _mm_sub_ps(_mm_setzero_ps(), wwww2);

    __m128 xInsideMax1 = _mm_cmple_ps(xxxx1, wwww1);
    __m128 xInsideMin1 = _mm_cmpge_ps(xxxx1, wwww1Neg);                                            
    __m128 yInsideMax1 = _mm_cmple_ps(yyyy1, wwww1);
    __m128 yInsideMin1 = _mm_cmpge_ps(yyyy1, wwww1Neg);                                     
    __m128 zInsideMax1 = _mm_cmple_ps(zzzz1, wwww1);
    __m128 zInsideMin1 = _mm_cmpge_ps(zzzz1, _mm_setzero_ps());     

    __m128 xInsideMax2 = _mm_cmple_ps(xxxx2, wwww2);
    __m128 xInsideMin2 = _mm_cmpge_ps(xxxx2, wwww2Neg);                                             
    __m128 yInsideMax2 = _mm_cmple_ps(yyyy2, wwww2);
    __m128 yInsideMin2 = _mm_cmpge_ps(yyyy2, wwww2Neg);  
    __m128 zInsideMax2 = _mm_cmple_ps(zzzz2, wwww2);
    __m128 zInsideMin2 = _mm_cmpge_ps(zzzz2, _mm_setzero_ps());

    // This part is platform specific
    // We just want to end up with an integer per plane to
    // show which point is inside the plane per bit
    int inside = _mm_movemask_ps(xInsideMax1) << 4;
    pInsidePlaneMask[0] = inside |_mm_movemask_ps(xInsideMax2);

    inside = _mm_movemask_ps(xInsideMin1) << 4;
    pInsidePlaneMask[1] = inside | _mm_movemask_ps(xInsideMin2);

    inside = _mm_movemask_ps(yInsideMax1) << 4;
    pInsidePlaneMask[2] = inside | _mm_movemask_ps(yInsideMax2);

    inside = _mm_movemask_ps(yInsideMin1) << 4;
    pInsidePlaneMask[3] = inside | _mm_movemask_ps(yInsideMin2);

    inside = _mm_movemask_ps(zInsideMax1) << 4;
    pInsidePlaneMask[4] = inside | _mm_movemask_ps(zInsideMax2);

    inside = _mm_movemask_ps(zInsideMin1) << 4;
    pInsidePlaneMask[5] = inside | _mm_movemask_ps(zInsideMin2);  
}

_FORCE_INLINE boolean IntersectsClipSpacePlanes(int* pInsidePlaneMask)
{
    return (pInsidePlaneMask[0] > 0) & (pInsidePlaneMask[1] > 0) &
           (pInsidePlaneMask[2] > 0) & (pInsidePlaneMask[3] > 0) &
           (pInsidePlaneMask[4] > 0) & (pInsidePlaneMask[5] > 0);
}

_FORCE_INLINE boolean ContainsClipSpacePlanes(int* pInsidePlaneMask)
{
    return (pInsidePlaneMask[0] & pInsidePlaneMask[1] &
           pInsidePlaneMask[2] & pInsidePlaneMask[3] &
           pInsidePlaneMask[4] & pInsidePlaneMask[5]) == 0xFF;
}


_NAMESPACE_END
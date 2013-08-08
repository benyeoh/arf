/**
 *  @file       LmSphere_PC.cpp
 *  @brief
 *
 *	@date       2013/7/31
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "Math/LmVector3.h"
#include "Math/LmAABB.h"
#include "Math/LmOrientedBB.h"
#include "Math/LmSphere.h"

LM_NS_BEGIN_MATH

inline LmBool LmSphere::Intersects(const LmAABB& other) const
{
    return other.Intersects(*this);
}

inline LmBool LmSphere::Contains(const LmAABB& other) const
{
    __m128 center	= Center().Vec128();
    __m128 boxMin	= other.Min().Vec128();
    __m128 boxMax	= other.Max().Vec128();

    // Find the farthest point of the AABB from the center of the sphere
    __m128 boxCenter			= _mm_mul_ps(_mm_sub_ps(boxMax, boxMin), _mm_set1_ps(0.5f));
    __m128 isLessThanEqCenter	= _mm_cmple_ps(center, boxCenter);
    __m128 isGreaterThanCenter	= _mm_cmpgt_ps(center, boxCenter);
    __m128 maxDistPoint			= _mm_and_ps(boxMax, isLessThanEqCenter);
    __m128 minDistPoint			= _mm_and_ps(boxMin, isGreaterThanCenter);
    __m128 farthestPoint		= _mm_or_ps(maxDistPoint, minDistPoint);
    __m128 dist					= _mm_sub_ps(center, farthestPoint);

#if LM_SSE4
    dist                = _mm_dp_ps(dist, dist, 0x7f);
#else
    dist				= _mm_mul_ps(dist, dist);

    // Collapse to get the total distance
    __m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
    dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
    collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
    dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;
#endif

    // And check against the radius of the sphere
    LmFloat sphereRadius = Radius();
    __m128 radius		= _mm_load_ss(&sphereRadius);
    radius				= _mm_mul_ss(radius, radius);
    dist				= _mm_cmple_ss(dist, radius);

    LmUint32 isContained;
    _mm_store_ss((LmFloat*) &isContained, dist);

    return isContained > 0;
}

inline LmBool LmSphere::Intersects(const LmOrientedBB& other) const
{
    const static LM_ALIGN16 LmFloat ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	
    
    // Project center to OBB space
    LmVector3 centerXYZ;
    LmVector3 lengthSq;

    LmVector3 centerFromOBB  = Center() - other.Center();

#ifdef LM_SSE4
    __m128 centerX      = _mm_dp_ps(other.XHalfExtent().Vec128(), centerFromOBB.Vec128(), 0x7f);
    __m128 centerY      = _mm_dp_ps(other.YHalfExtent().Vec128(), centerFromOBB.Vec128(), 0x7f);
    __m128 centerZ      = _mm_dp_ps(other.ZHalfExtent().Vec128(), centerFromOBB.Vec128(), 0x7f);
    centerXYZ.Vec128()  = _mm_unpacklo_ps(centerX, centerY);                // r0 = dpX.x, r1 = dpY.x
    centerXYZ.Vec128()  = _mm_movelh_ps(centerXYZ.Vec128(), centerZ);       // r0 = dpX.x, r1 = dpY.x, r2 = dpZ.x
    
    // Find length sq
    __m128 dpX          = _mm_dp_ps(other.XHalfExtent().Vec128(), other.XHalfExtent().Vec128(), 0x7f);
    __m128 dpY          = _mm_dp_ps(other.YHalfExtent().Vec128(), other.YHalfExtent().Vec128(), 0x7f);
    __m128 dpZ          = _mm_dp_ps(other.ZHalfExtent().Vec128(), other.ZHalfExtent().Vec128(), 0x7f);
    lengthSq.Vec128()   = _mm_unpacklo_ps(dpX, dpY);                // r0 = dpX.x, r1 = dpY.x
    lengthSq.Vec128()   = _mm_movelh_ps(lengthSq.Vec128(), dpZ);    // r0 = dpX.x, r1 = dpY.x, r2 = dpZ.x
#else
    centerXYZ.X()       = other.XHalfExtent().Dot(centerFromOBB.Vec128());
    centerXYZ.Y()       = other.YHalfExtent().Dot(centerFromOBB.Vec128());
    centerXYZ.Z()       = other.ZHalfExtent().Dot(centerFromOBB.Vec128());
    
    lengthSq.X()        = other.XHalfExtent().Dot(other.XHalfExtent());
    lengthSq.Y()        = other.YHalfExtent().Dot(other.YHalfExtent());
    lengthSq.Z()        = other.ZHalfExtent().Dot(other.ZHalfExtent());
#endif

    // Compute the distance to the closest point of OBB (in OBB space)
    __m128 maxDiff      = _mm_max_ps( _mm_sub_ps(centerXYZ.Vec128(), lengthSq.Vec128()), *((__m128*) ZERO) );
    __m128 negLengthSq  = _mm_sub_ps(*((__m128*) ZERO), lengthSq.Vec128());
    __m128 minDiff      = _mm_max_ps( _mm_sub_ps(negLengthSq, centerXYZ.Vec128()), *((__m128*) ZERO) );
    
    __m128 maxExtents   = _mm_add_ps(maxDiff, minDiff);
    maxExtents          = _mm_mul_ps(maxExtents, _mm_rsqrt_ps(lengthSq.Vec128()));
 
#ifdef LM_SSE4
    __m128 dist         = _mm_dp_ps(maxExtents, maxExtents, 0x7F);
#else
    __m128 dist			= _mm_mul_ps(maxExtents, maxExtents);

    // Collapse to get the total distance
    __m128 collapseDist	= _mm_movehl_ps(dist, dist);		// r0 = z;
    dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + z;
    collapseDist		= _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 0, 1));	// r0 = y;
    dist				= _mm_add_ss(dist, collapseDist);	// r0 = x + y + z;
#endif

    LmFloat sphereRadius = Radius();
    __m128 radius		= _mm_load_ss(&sphereRadius);
    radius				= _mm_mul_ss(radius, radius);
    dist				= _mm_cmple_ss(dist, radius);

    LmUint32 isIntersect;
    _mm_store_ss((LmFloat*) &isIntersect, dist);

    return isIntersect > 0;
}


LM_NS_END_MATH
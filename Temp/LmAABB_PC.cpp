/**
 *  @file       LmAABB_PC.cpp
 *  @brief
 *
 *	@date       2013/7/30
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "Math/LmVector3.h"
#include "Math/LmAABB.h"
#include "Math/LmOrientedBB.h"
#include "Math/LmSphere.h"
#include "Math/LmMatrix.h"
#include "Math/LmMatrix4.h"

LM_NS_BEGIN_MATH

inline LmBool LmAABB::Intersects(const LmAABB& other) const
{
    __m128 isLessThan       = _mm_cmplt_ps(Min().Vec128(), other.Max().Vec128());
    __m128 isGreaterThan    = _mm_cmpgt_ps(Max().Vec128(), other.Min().Vec128());

    __m128 res  = _mm_and_ps(isLessThan, isGreaterThan);
    LmInt32 val = _mm_movemask_ps(res);
    return ((val & 0x7) == 0x7);
}

inline LmBool LmAABB::Contains(const LmAABB& other) const
{
    __m128 isLessThan       = _mm_cmplt_ps(other.Max().Vec128(), Max().Vec128());
    __m128 isGreaterThan    = _mm_cmpgt_ps(other.Min().Vec128(), Min().Vec128());

    __m128 res  = _mm_and_ps(isLessThan, isGreaterThan);
    LmInt32 val = _mm_movemask_ps(res);
    return ((val & 0x7) == 0x7);
}

inline LmBool LmAABB::Intersects(const LmOrientedBB& other) const
{
    LmVector3 aabbExtents   = (Max() - Min()) * 0.5f;
    LmVector3 aabbCenter    = Min() + aabbExtents;
    LmVector3 centerDiff    = other.Center() - aabbCenter;

    // Perform separating axes test in AABB space
    // Project max extent of OBB to AABB space
    LmVector3 obbMaxExtents = LmVector3::Abs(other.XHalfExtent()) 
                            + LmVector3::Abs(other.YHalfExtent()) 
                            + LmVector3::Abs(other.ZHalfExtent());
    
    // Check if OBB max point intersects AABB
    LmVector3 aabbSeparatingLengths = obbMaxExtents + aabbExtents;
    __m128 hasSeparatingAxis = _mm_cmplt_ps(aabbSeparatingLengths.Vec128(), LmVector3::Abs(centerDiff).Vec128());

    // Exit early if there is at least one separating axis
    LmInt32 val = _mm_movemask_ps(hasSeparatingAxis);
    if((val & 0x7) > 0)
        return false;

    // Perform separating axes test in OBB space
    // Project max aabb extents to OBB space

    // There's some shortcuts here because the AABB extents are always (x, 0, 0) , (0, y , 0) and (0, 0, z)
    // We want (for each axis):
    //      max( dot(X, +-Ex +-Ey +-Ez) ) + dot(X, X) >= abs( dot(X, C1-C2) ), where X is one axis
    //  =   max( +- dot(X, Ex) +- dot(X, Ey) +- dot(X, Ez) ) ...
    //  =   abs( +- dot(X, Ex) ) + abs( +- dot(X, Ey) ) + abs( +- dot(X, Ez) ) ...

    LmVector3 aabbOnObbX = other.XHalfExtent() * aabbExtents;
    aabbOnObbX = LmVector3::Abs(aabbOnObbX);

    LmVector3 aabbOnObbY = other.YHalfExtent() * aabbExtents;
    aabbOnObbY = LmVector3::Abs(aabbOnObbY);

    LmVector3 aabbOnObbZ = other.ZHalfExtent() * aabbExtents;
    aabbOnObbZ = LmVector3::Abs(aabbOnObbZ);
 
    __m128 obbSeparatingX   = _mm_unpacklo_ps(aabbOnObbX.Vec128(), aabbOnObbY.Vec128());    // r0 = X.x, r1 = Y.x
    obbSeparatingX          = _mm_movelh_ps(obbSeparatingX, aabbOnObbZ.Vec128());           // r0 = X.x, r1 = Y.x, r2 = Z.x

    __m128 obbSeparatingY   = _mm_unpacklo_ps(aabbOnObbX.Vec128(), aabbOnObbY.Vec128());                    // r2 = X.y, r3 = Y.y
    obbSeparatingY          = _mm_shuffle_ps(obbSeparatingY, aabbOnObbZ.Vec128(), _MM_SHUFFLE(1, 1, 3, 2)); // r0 = X.y, r1 = Y.y, r2 = Z.y

    __m128 obbSeparatingZ   = _mm_unpackhi_ps(aabbOnObbX.Vec128(), aabbOnObbY.Vec128());                    // r0 = X.z, r1 = Y.z
    obbSeparatingZ          = _mm_shuffle_ps(obbSeparatingZ, aabbOnObbZ.Vec128(), _MM_SHUFFLE(2, 2, 1, 0)); // r0 = X.z, r1 = Y.z, r2 = Z.z  
    
    LmVector3 obbSeparatingLengths;
    obbSeparatingLengths.Vec128() = _mm_add_ps( _mm_add_ps(obbSeparatingX, obbSeparatingY), obbSeparatingZ );

    //obbSeparatingLengths.X() = aabbOnObbX.X() + aabbOnObbX.Y() + aabbOnObbX.Z();
    //obbSeparatingLengths.Y() = aabbOnObbY.X() + aabbOnObbY.Y() + aabbOnObbY.Z();
    //obbSeparatingLengths.Z() = aabbOnObbZ.X() + aabbOnObbZ.Y() + aabbOnObbZ.Z();
   
#if LM_SSE4
    // dot(X, X)
    __m128 dpX  = _mm_dp_ps(other.XHalfExtent().Vec128(), other.XHalfExtent().Vec128(), 0x7f);
    __m128 dpY  = _mm_dp_ps(other.YHalfExtent().Vec128(), other.YHalfExtent().Vec128(), 0x7f);
    __m128 dpZ  = _mm_dp_ps(other.ZHalfExtent().Vec128(), other.ZHalfExtent().Vec128(), 0x7f);
    __m128 dp   = _mm_unpacklo_ps(dpX, dpY);    // r0 = dpX.x, r1 = dpY.x
    dp          = _mm_movelh_ps(dp, dpZ);       // r0 = dpX.x, r1 = dpY.x, r2 = dpZ.x
    obbSeparatingLengths.Vec128() = _mm_add_ps(obbSeparatingLengths.Vec128(), dp);
#else
    obbSeparatingLengths.X() += other.XHalfExtent().Dot(other.XHalfExtent());
    obbSeparatingLengths.Y() += other.YHalfExtent().Dot(other.YHalfExtent());
    obbSeparatingLengths.Z() += other.ZHalfExtent().Dot(other.ZHalfExtent());
#endif

    // Then check against centerDiff for each axis
    LmVector3 obbCenterDiffs;

#if LM_SSE4
    // dot(X, C1-C0)
    __m128 dpCenterDiffX    = _mm_dp_ps(other.XHalfExtent().Vec128(), centerDiff.Vec128(), 0x7f);
    __m128 dpCenterDiffY    = _mm_dp_ps(other.YHalfExtent().Vec128(), centerDiff.Vec128(), 0x7f);
    __m128 dpCenterDiffZ    = _mm_dp_ps(other.ZHalfExtent().Vec128(), centerDiff.Vec128(), 0x7f);
    __m128 dpCenterDiff     = _mm_unpacklo_ps(dpCenterDiffX, dpCenterDiffY);    // r0 = dpX.x, r1 = dpY.x
    obbCenterDiffs.Vec128() = _mm_movelh_ps(dpCenterDiff, dpCenterDiffZ);       // r0 = dpX.x, r1 = dpY.x, r2 = dpZ.x
#else
    obbCenterDiffs.X() = other.XHalfExtent().Dot(centerDiff);
    obbCenterDiffs.Y() = other.YHalfExtent().Dot(centerDiff);
    obbCenterDiffs.Z() = other.ZHalfExtent().Dot(centerDiff);
#endif

    obbCenterDiffs = LmVector3::Abs(obbCenterDiffs);
    
    // Check if there is at least one separating axis
    hasSeparatingAxis   = _mm_cmplt_ps(obbSeparatingLengths.Vec128(), obbCenterDiffs.Vec128());
    val                 = _mm_movemask_ps(hasSeparatingAxis);
    return ((val & 0x7) == 0);
}

inline LmBool LmAABB::Intersects(const LmSphere& other) const
{
    const static LM_ALIGN16 LmFloat ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	
    
    __m128 center	= other.Center().Vec128();
    __m128 boxMin	= Min().Vec128();
    __m128 boxMax	= Max().Vec128();

    __m128 centerToMin	= _mm_sub_ps(boxMin, center);
    centerToMin			= _mm_max_ps(centerToMin, *((__m128*)ZERO));

    __m128 maxToCenter	= _mm_sub_ps(center, boxMax);
    maxToCenter			= _mm_max_ps(maxToCenter, *((__m128*)ZERO));

    __m128 dist			= _mm_add_ps(centerToMin , maxToCenter);

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
    
    LmFloat sphereRadius = other.Radius();
    __m128 radius		= _mm_load_ss(&sphereRadius);
    radius				= _mm_mul_ss(radius, radius);
    dist				= _mm_cmple_ss(dist, radius);

    LmUint32 isIntersect;
    _mm_store_ss((LmFloat*) &isIntersect, dist);

    return isIntersect > 0;
}

inline LmAABB LmAABB::TransformOrthogonalToAABB(LmMatrix4Arg mat) const
{
    LmVector3 aabbExtents   = (Max() - Min()) * 0.5f;
    LmVector3 aabbCenter    = Min() + aabbExtents;
 
    LmVector3 aabbCenterPost    = LmMatrix4::Transform(aabbCenter, mat);
    //LmVector3 aabbExtentsPost   = LmMatrix4::TransformNormal(aabbExtents, mat);

    // C.M + [abs(x.m00), abs(x.m01), abs(x.m02)] + [abs(y.m10), abs(y.m11), abs(y.m12] + [abs(z.m20), abs(z.m21), abs(z.m22)]

    __m128 aabbExtentsPostX     = _mm_shuffle_ps(aabbExtents.Vec128(), aabbExtents.Vec128(), _MM_SHUFFLE(0, 0, 0, 0));
    aabbExtentsPostX            = _mm_mul_ps(mat.GetXAxis().Vec128(), aabbExtentsPostX);
    aabbExtentsPostX            = _mm_andnot_ps(_mm_set1_ps(-0.0f), aabbExtentsPostX);

    __m128 aabbExtentsPostY     = _mm_shuffle_ps(aabbExtents.Vec128(), aabbExtents.Vec128(), _MM_SHUFFLE(1, 1, 1, 1));
    aabbExtentsPostY            = _mm_mul_ps(mat.GetYAxis().Vec128(), aabbExtentsPostY);
    aabbExtentsPostY            = _mm_andnot_ps(_mm_set1_ps(-0.0f), aabbExtentsPostY);

    __m128 aabbExtentsPostZ     = _mm_shuffle_ps(aabbExtents.Vec128(), aabbExtents.Vec128(), _MM_SHUFFLE(2, 2, 2, 2));
    aabbExtentsPostZ            = _mm_mul_ps(mat.GetZAxis().Vec128(), aabbExtentsPostZ);
    aabbExtentsPostZ            = _mm_andnot_ps(_mm_set1_ps(-0.0f), aabbExtentsPostZ);

    __m128 aabbMaxExtentsAbs    = _mm_add_ps(aabbExtentsPostX, aabbExtentsPostY);
    aabbMaxExtentsAbs           = _mm_add_ps(aabbMaxExtentsAbs, aabbExtentsPostZ);

    __m128 maxPost  = _mm_add_ps(aabbCenterPost.Vec128(), aabbMaxExtentsAbs);
    __m128 minPost  = _mm_sub_ps(aabbCenterPost.Vec128(), aabbMaxExtentsAbs);

    LmAABB res;
    res.Min().Vec128() = minPost;
    res.Max().Vec128() = maxPost;

    return res;
}

inline void LmAABB::TransformToPoints(LmMatrix4Arg mat, LmVector4 toStore[8]) const
{
    // Get the extents 
    __m128 extents = _mm_sub_ps(Max().Vec128(), Min().Vec128());
   
    // Broadcast the corner point
    __m128 cornerY = _mm_shuffle_ps(Min().Vec128(), Min().Vec128(), _MM_SHUFFLE(1, 1, 1, 1));
    __m128 cornerZ = _mm_shuffle_ps(Min().Vec128(), Min().Vec128(), _MM_SHUFFLE(2, 2, 2, 2));
    __m128 cornerX = _mm_shuffle_ps(Min().Vec128(), Min().Vec128(), _MM_SHUFFLE(0, 0, 0, 0));

    // Transform the corner point
    cornerX						= _mm_mul_ps(cornerX, mat.GetXAxis().Vec128());
    cornerY						= _mm_mul_ps(cornerY, mat.GetYAxis().Vec128());
    cornerZ						= _mm_mul_ps(cornerZ, mat.GetZAxis().Vec128());
    cornerX						= _mm_add_ps(cornerX, cornerZ);
    cornerY						= _mm_add_ps(cornerY, mat.GetTranslation().Vec128());
    __m128 transformedCorner	= _mm_add_ps(cornerX, cornerY);

    // Broadcast the extents
    __m128 extentX	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 extentZ	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 extentY	= _mm_shuffle_ps(extents, extents, _MM_SHUFFLE(1, 1, 1, 1));

    // Transform the extents
    __m128 transformedX = _mm_mul_ps(extentX, mat.GetXAxis().Vec128());
    __m128 transformedY = _mm_mul_ps(extentY, mat.GetYAxis().Vec128());
    __m128 transformedZ = _mm_mul_ps(extentZ, mat.GetZAxis().Vec128());

    // Now we get the eight points of the OBB
    __m128 dx = _mm_add_ps(transformedCorner, transformedX);	// dx
    __m128 dy = _mm_add_ps(transformedCorner, transformedY);	// dy
    __m128 dxdy = _mm_add_ps(dx, transformedY);	// dx + dy

    __m128 dz = _mm_add_ps(transformedCorner, transformedZ);	// dz
    __m128 dxdz = _mm_add_ps(dx, transformedZ);	// dx + dz
    __m128 dydz = _mm_add_ps(dy, transformedZ);	// dy + dz
    __m128 dxdydz = _mm_add_ps(dxdy, transformedZ);	// dx + dy + dz

    toStore[0].Vec128() = transformedCorner;
    toStore[1].Vec128() = dx;
    toStore[2].Vec128() = dy;
    toStore[3].Vec128() = dxdy;
    toStore[4].Vec128() = dz;
    toStore[5].Vec128() = dxdz;
    toStore[6].Vec128() = dydz;
    toStore[7].Vec128() = dxdydz;
}

inline LmOrientedBB LmAABB::TransformOrthogonalToOrientedBB(LmMatrix4Arg mat) const
{
    LmVector3 diff      = (Max() - Min()) * 0.5f;
    LmVector3 center    = Min() + diff;

    LmOrientedBB res;
    res.Center()    = LmMatrix4::Transform(center, mat);
    res.XHalfExtent().Vec128()  = _mm_mul_ps( _mm_shuffle_ps(diff.Vec128(), diff.Vec128(), _MM_SHUFFLE(0, 0, 0, 0)), mat.GetXAxis().Vec128() );
    res.YHalfExtent().Vec128()  = _mm_mul_ps( _mm_shuffle_ps(diff.Vec128(), diff.Vec128(), _MM_SHUFFLE(1, 1, 1, 1)), mat.GetYAxis().Vec128() );
    res.ZHalfExtent().Vec128()  = _mm_mul_ps( _mm_shuffle_ps(diff.Vec128(), diff.Vec128(), _MM_SHUFFLE(2, 2, 2, 2)), mat.GetZAxis().Vec128() );

    return res;
}

LM_NS_END_MATH
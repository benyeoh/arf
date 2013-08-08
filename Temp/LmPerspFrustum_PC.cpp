/**
 *  @file       LmPerspFrustum_PC.cpp
 *  @brief
 *
 *	@date       2013/8/1
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
#include "Math/LmPerspFrustum.h"

LM_NS_BEGIN_MATH

inline LmAABB LmPerspFrustum::ToAABBFromClipSpacePoints(LmVector4 clipSpacePoints[8]) const
{
    clipSpacePoints[0].Vec128() = _mm_max_ps(clipSpacePoints[0].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[1].Vec128() = _mm_max_ps(clipSpacePoints[1].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[2].Vec128() = _mm_max_ps(clipSpacePoints[2].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[3].Vec128() = _mm_max_ps(clipSpacePoints[3].Vec128(), m_NearClamp.Vec128());

    clipSpacePoints[4].Vec128() = _mm_max_ps(clipSpacePoints[4].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[5].Vec128() = _mm_max_ps(clipSpacePoints[5].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[6].Vec128() = _mm_max_ps(clipSpacePoints[6].Vec128(), m_NearClamp.Vec128());
    clipSpacePoints[7].Vec128() = _mm_max_ps(clipSpacePoints[7].Vec128(), m_NearClamp.Vec128());

    // Do projection with linear Z
    __m128 tempUnit = m_FarLinearDiv.Vec128();
    __m128 wXmm = _mm_shuffle_ps(clipSpacePoints[0].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[0].Vec128() = _mm_mul_ps(clipSpacePoints[0].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[1].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); // r3=farZ r2=farZ r1=w r0=w
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[1].Vec128() = _mm_mul_ps(clipSpacePoints[1].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[2].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[2].Vec128() = _mm_mul_ps(clipSpacePoints[2].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[3].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[3].Vec128() = _mm_mul_ps(clipSpacePoints[3].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[4].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[4].Vec128() = _mm_mul_ps(clipSpacePoints[4].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[5].Vec128(), tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[5].Vec128() = _mm_mul_ps(clipSpacePoints[5].Vec128(), wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[6].Vec128() , tempUnit, _MM_SHUFFLE(0, 0,  3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[6].Vec128() = _mm_mul_ps(clipSpacePoints[6].Vec128() , wXmm);

    wXmm = _mm_shuffle_ps(clipSpacePoints[7].Vec128() , tempUnit, _MM_SHUFFLE(0, 0, 3, 3)); 
    wXmm = _mm_rcp_ps(wXmm);
    clipSpacePoints[7].Vec128()  = _mm_mul_ps(clipSpacePoints[7].Vec128() , wXmm);

    // Now find the AABox
    LmAABB bounds;
    bounds.Min().Vec128() = _mm_min_ps(clipSpacePoints[0].Vec128() , clipSpacePoints[1].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(clipSpacePoints[0].Vec128() , clipSpacePoints[1].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[2].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[2].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[3].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[3].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[4].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[4].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[5].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[5].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[6].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[6].Vec128() );
    bounds.Min().Vec128() = _mm_min_ps(bounds.Min().Vec128(), clipSpacePoints[7].Vec128() );
    bounds.Max().Vec128() = _mm_max_ps(bounds.Max().Vec128(), clipSpacePoints[7].Vec128() );

    return bounds;
}

LM_NS_END_MATH
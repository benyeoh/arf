/**
 *  @file       LmOrthoFrusutm.cpp
 *  @brief
 *
 *	@date       2013/7/30
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "LmVector3.h"
#include "LmAABB.h"
#include "LmOrientedBB.h"
#include "LmSphere.h"
#include "LmMatrix.h"
#include "LmMatrix4.h"
#include "LmOrthoFrustum.h"

LM_NS_BEGIN_MATH

void LmOrthoFrustum::Set(LmMatrix4Arg viewProj, LmMatrix4Arg invViewProj)
{
    m_ViewProj      = viewProj;
    m_InvViewProj   = invViewProj;

    m_ClipSpaceBounds.Max().Set(1.0f, 1.0f, 1.0f);
    m_ClipSpaceBounds.Min().Set(-1.0f, -1.0f, 0.0f);
}

inline LmBool LmOrthoFrustum::Intersects(const LmOrientedBB& other) const
{
    LmAABB otherClipSpaceBounds = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Intersects(otherClipSpaceBounds);
}

inline LmBool LmOrthoFrustum::Intersects(const LmAABB& other) const
{
    LmAABB otherClipSpaceBounds = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Intersects(otherClipSpaceBounds);
}

inline LmBool LmOrthoFrustum::Contains(const LmOrientedBB& other) const
{
    LmAABB otherClipSpaceBounds = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Contains(otherClipSpaceBounds);
}

inline LmBool LmOrthoFrustum::Contains(const LmAABB& other) const
{
    LmAABB otherClipSpaceBounds = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Contains(otherClipSpaceBounds);
}

inline LmBool LmOrthoFrustum::IntersectsStoreClipSpaceRes(const LmAABB& other, LmAABB& resClipSpace) const
{
    resClipSpace = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Intersects(resClipSpace);
}

inline LmBool LmOrthoFrustum::IntersectsStoreClipSpaceRes(const LmOrientedBB& other, LmAABB& resClipSpace) const
{
    resClipSpace = other.TransformOrthogonalToAABB(m_ViewProj);
    return m_ClipSpaceBounds.Intersects(resClipSpace);
}

inline LmBool LmOrthoFrustum::ContainsClipSpace(const LmAABB& otherClipSpace) const
{
    return m_ClipSpaceBounds.Contains(otherClipSpace);
}

LmAABB  LmOrthoFrustum::ToAABB() const
{
    LmVector3 frustumPoints[8];
    frustumPoints[0] = LmVector3(-1.0f, 1.0f, 0.0f);
    frustumPoints[1] = LmVector3(1.0f, 1.0f, 0.0f);
    frustumPoints[2] = LmVector3(-1.0f, -1.0f, 0.0f);
    frustumPoints[3] = LmVector3(1.0f, -1.0f, 0.0f);

    frustumPoints[4] = LmVector3(-1.0f, 1.0f, 1.0f);
    frustumPoints[5] = LmVector3(1.0f, 1.0f, 1.0f);
    frustumPoints[6] = LmVector3(-1.0f, -1.0f, 1.0f);
    frustumPoints[7] = LmVector3(1.0f, -1.0f, 1.0f);

    LmAABB bounds;
    bounds.Max() = LmVector3(-FLT_MAX);
    bounds.Min() = LmVector3(FLT_MAX);

    for(LmUint32 i=0; i < 8; i++)
    {
        LmVector3 frustumPointWorld = LmMatrix4::Transform(frustumPoints[i], m_InvViewProj);
        bounds.Min() = LmVector3::CompMin(bounds.Min(), frustumPointWorld);
        bounds.Max() = LmVector3::CompMax(bounds.Max(), frustumPointWorld);
    }

    return bounds;
}

LM_NS_END_MATH
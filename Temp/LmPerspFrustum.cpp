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

void LmPerspFrustum::Set(LmMatrix4Arg viewProj, LmMatrix4Arg invViewProj, LmFloat nearPlane, LmFloat farPlane)
{
    m_ViewProj      = viewProj;
    m_InvViewProj   = invViewProj;

    m_ClipSpaceBounds.Max().Set(1.0f, 1.0f, 1.0f);
    m_ClipSpaceBounds.Min().Set(-1.0f, -1.0f, 0.0f);

    m_NearClamp.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX, nearPlane);
    m_FarLinearDiv.Set(farPlane, farPlane, farPlane, farPlane);
}

inline LmBool LmPerspFrustum::Intersects(const LmOrientedBB& other) const
{
    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    LmAABB otherClipSpaceBounds = ToAABBFromClipSpacePoints(otherClipSpacePoints);   
    return m_ClipSpaceBounds.Intersects(otherClipSpaceBounds);
}

inline LmBool LmPerspFrustum::Intersects(const LmAABB& other) const
{
    // TODO: Use frustum planes instead if clipspace AABB is not required!!!
    // Frustum planes are much faster

    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    LmAABB otherClipSpaceBounds = ToAABBFromClipSpacePoints(otherClipSpacePoints);   
    return m_ClipSpaceBounds.Intersects(otherClipSpaceBounds);
}

inline LmBool LmPerspFrustum::Contains(const LmOrientedBB& other) const
{
    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    LmAABB otherClipSpaceBounds = ToAABBFromClipSpacePoints(otherClipSpacePoints);   
    return m_ClipSpaceBounds.Contains(otherClipSpaceBounds);
}

inline LmBool LmPerspFrustum::Contains(const LmAABB& other) const
{
    // TODO: Use frustum planes instead if clipspace AABB is not required!!!
    // Frustum planes are much faster

    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    LmAABB otherClipSpaceBounds = ToAABBFromClipSpacePoints(otherClipSpacePoints);   
    return m_ClipSpaceBounds.Contains(otherClipSpaceBounds);
}

inline LmBool LmPerspFrustum::IntersectsStoreClipSpaceRes(const LmAABB& other, LmAABB& resClipSpace) const
{
    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    resClipSpace = ToAABBFromClipSpacePoints(otherClipSpacePoints);    
    return m_ClipSpaceBounds.Intersects(resClipSpace);
}

inline LmBool LmPerspFrustum::IntersectsStoreClipSpaceRes(const LmOrientedBB& other, LmAABB& resClipSpace) const
{
    LmVector4 otherClipSpacePoints[8];
    other.TransformToPoints(m_ViewProj, otherClipSpacePoints);

    resClipSpace = ToAABBFromClipSpacePoints(otherClipSpacePoints);    
    return m_ClipSpaceBounds.Intersects(resClipSpace);
}

inline LmBool LmPerspFrustum::ContainsClipSpace(const LmAABB& otherClipSpace) const
{
    return m_ClipSpaceBounds.Contains(otherClipSpace);
}

LmAABB  LmPerspFrustum::ToAABB() const
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
        LmVector3 frustumPointWorld = LmMatrix4::TransformCoordinate(frustumPoints[i], m_InvViewProj);
        bounds.Min() = LmVector3::CompMin(bounds.Min(), frustumPointWorld);
        bounds.Max() = LmVector3::CompMax(bounds.Max(), frustumPointWorld);
    }

    return bounds;
}

LM_NS_END_MATH
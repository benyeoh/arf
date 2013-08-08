/**
 *  @file       LmSphere.cpp
 *  @brief
 *
 *	@date       2013/7/30
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "LmVector3.h"
#include "LmAABB.h"
#include "LmSphere.h"

LM_NS_BEGIN_MATH

inline LmAABB LmSphere::ToAABB() const
{
    LmAABB aabb;
    aabb.Min() = Center() - LmVector3(m_Radius);
    aabb.Max() = Center() + LmVector3(m_Radius);

    return aabb;
}

LM_NS_END_MATH

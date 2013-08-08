/**
 *  @file       LmAABB.cpp
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

void LmAABB::TransformOrthogonal(LmMatrix4Arg mat)
{
    *this = TransformOrthogonalToAABB(mat);
}

inline LmOrientedBB LmAABB::ToOrientedBB() const
{
    LmVector3 diff      = (Max() - Min()) * 0.5f;
    LmVector3 center    = Min() + diff;

    LmOrientedBB res;
    res.Center()        = center;
    res.XHalfExtent().Set(diff.X(), 0.0f, 0.0f);
    res.YHalfExtent().Set(0.0f, diff.Y(), 0.0f);
    res.ZHalfExtent().Set(0.0f, 0.0f, diff.Z());

    return res;

}

LM_NS_END_MATH
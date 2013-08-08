/**
 *  @file       LmOrientedBB.cpp
 *  @brief
 *
 *	@date       2013/7/29
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "LmVector3.h"
#include "LmOrientedBB.h"
#include "LmAABB.h"

LM_NS_BEGIN_MATH

LmFloat LmOrientedBB::GetXExtentLengthSq() const
{
    return m_XHalfExtent.LengthSq();
}

LmFloat LmOrientedBB::GetYExtentLengthSq() const
{
    return m_YHalfExtent.LengthSq();
}

LmFloat LmOrientedBB::GetZExtentLengthSq() const
{
    return m_ZHalfExtent.LengthSq();
}

LmVector3 LmOrientedBB::GetHalfExtentsLength() const
{
    LmVector3 res;
    res.X() = m_XHalfExtent.LengthSq();
    res.Y() = m_YHalfExtent.LengthSq();
    res.Z() = m_ZHalfExtent.LengthSq();

    LmVector3 rcpSqrt = LmVector3::RcpSqrt(res);
    res = LmVector3::Rcp(rcpSqrt);

    return res;
    //// 1 iteration of Newton-Rhapson
    //LmVector3 mid(3.0f);
    //LmVector3 resRcpSqrt = LmVector3::Mul(res, rcpSqrt);
    //resRcpSqrt = LmVector3::Mul(resRcpSqrt, rcpSqrt);

    //res = LmVector3::Mul( LmVector3::Mul(rcpSqrt, 0.5f), LmVector3::Sub(mid, resRcpSqrt) );
    //return res;
}

inline LmAABB LmOrientedBB::ToAABB() const
{
    LmVector3 extentsXPost      = LmVector3::Abs(XHalfExtent());
    LmVector3 extentsYPost      = LmVector3::Abs(YHalfExtent());
    LmVector3 extentsZPost      = LmVector3::Abs(ZHalfExtent());

    LmVector3 obbMaxExtentsAbs  = extentsXPost + extentsYPost;
    obbMaxExtentsAbs            = obbMaxExtentsAbs + extentsZPost;

    LmAABB res;
    res.Min() = Center() - obbMaxExtentsAbs;
    res.Max() = Center() + obbMaxExtentsAbs;

    return res;
}

inline LmAABB LmOrientedBB::TransformOrthogonalToAABB(LmMatrix4Arg mat) const
{
    LmVector3 obbCenterPost    = LmMatrix4::Transform(Center(), mat);

    // C.M +- (abs(X.M) + abs(Y.M) + abs(Z.M))

    LmVector3 extentsXPost      = LmMatrix4::TransformNormal(XHalfExtent(), mat);
    extentsXPost                = LmVector3::Abs(extentsXPost);
    LmVector3 extentsYPost      = LmMatrix4::TransformNormal(YHalfExtent(), mat);
    extentsYPost                = LmVector3::Abs(extentsYPost);
    LmVector3 extentsZPost      = LmMatrix4::TransformNormal(ZHalfExtent(), mat);
    extentsZPost                = LmVector3::Abs(extentsZPost);

    LmVector3 obbMaxExtentsAbs  = extentsXPost + extentsYPost;
    obbMaxExtentsAbs            = obbMaxExtentsAbs + extentsZPost;

    LmAABB res;
    res.Min() = obbCenterPost - obbMaxExtentsAbs;
    res.Max() = obbCenterPost + obbMaxExtentsAbs;

    return res;
}

inline LmOrientedBB LmOrientedBB::TransformOrthogonalToOrientedBB(LmMatrix4Arg mat) const
{
    LmOrientedBB obb;
    obb.Center() = LmMatrix4::Transform(Center(), mat);
    obb.XHalfExtent() = LmMatrix4::TransformNormal(XHalfExtent(), mat);
    obb.YHalfExtent() = LmMatrix4::TransformNormal(YHalfExtent(), mat);
    obb.ZHalfExtent() = LmMatrix4::TransformNormal(ZHalfExtent(), mat);
    
    return obb;
}

void LmOrientedBB::TransformOrthogonal(LmMatrix4Arg mat)
{
    *this = TransformOrthogonalToOrientedBB(mat);
}

LM_NS_END_MATH
//==============================================================================
//
//		TransformVecW1.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/10/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

extern inline gmtl::VecA4f* 
TransformVecW1(const gmtl::MatrixA44f* pMat, const float* pVecIn, gmtl::VecA4f* pVecOut);

extern inline gmtl::Vec4f* 
TransformVecW1(const gmtl::Matrix44f* pMat, const gmtl::Vec4f* pVecIn, gmtl::Vec4f* pVecOut);

extern inline gmtl::Vec3f* 
TransformVecW1(const gmtl::Matrix44f* pMat, const gmtl::Vec3f* pVecIn, gmtl::Vec3f* pVecOut);

extern inline gmtl::VecA4f* 
TransformVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut);

extern inline gmtl::VecA3f* 
TransformVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut);

_NAMESPACE_END
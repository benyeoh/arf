//==============================================================================
//
//		BatchTransformVec.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/1/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN 

extern void 
BatchTransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len);

extern void 
BatchTransformVec(const gmtl::Matrix44f* pMat, const gmtl::Vec4f* pVecIn, gmtl::Vec4f* pVecOut, uint len);

extern void 
BatchTransformVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut, uint len);

extern void
BatchTransformAndProjectVec(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len);


_NAMESPACE_END
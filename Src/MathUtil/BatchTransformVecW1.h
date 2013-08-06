//==============================================================================
//
//		BatchTransformVecW1.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/10/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN 

extern void 
BatchTransformVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len);

extern void 
BatchTransformVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut, uint len);

extern void 
BatchTransformVecW1(const gmtl::Matrix44f* pMat, const gmtl::Vec4f* pVecIn, gmtl::Vec4f* pVecOut, uint len);

extern void
BatchTransformAndProjectVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len);

extern void 
BatchTransformAndProjectVecW1(const gmtl::MatrixA44f* pMat, const gmtl::VecA3f* pVecIn, gmtl::VecA3f* pVecOut, uint len);

_NAMESPACE_END
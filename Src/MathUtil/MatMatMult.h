//==============================================================================
//
//		MatMatMult.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

extern inline gmtl::MatrixA44f* 
MatMatMult(gmtl::MatrixA44f* pResult, const gmtl::MatrixA44f* pLHS, const gmtl::MatrixA44f* pRHS);

extern inline gmtl::Matrix44f* 
MatMatMult(gmtl::Matrix44f* pResult, const gmtl::Matrix44f* pLHS, const gmtl::Matrix44f* pRHS);

_NAMESPACE_END

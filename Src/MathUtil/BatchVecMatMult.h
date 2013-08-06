//==============================================================================
//
//		BatchVecMatMult.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/1/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN 

extern void BatchVecMatMult(gmtl::Matrix44f* pMat, gmtl::VecA4f* pVecIn, gmtl::VecA4f* pVecOut, uint len);

_NAMESPACE_END
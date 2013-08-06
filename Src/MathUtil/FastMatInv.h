//================================================================================
//
//		FastMatInv.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		20/11/2011
//
//================================================================================

#pragma once

//extern inline gmtl::MatrixA33f* FastMatInv33(gmtl::MatrixA33f* pDest, const gmtl::MatrixA33f* pSrc);
extern inline gmtl::MatrixA44f* FastMatInv44(const gmtl::MatrixA44f* pSrc, gmtl::MatrixA44f* pDst);

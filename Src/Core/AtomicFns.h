//==============================================================================
//
//		AtomicFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/07/10
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

void* AtomicCompareAndSwapPtr(void** pDest, void* exchangeVal, void* compareVal);
int	AtomicCompareAndSwap(int* pDest, int exchangeVal, int compareVal);
int	AtomicSwap(int* pDest, int exchangeVal);
int	AtomicInc(int* pVal);
int AtomicDec(int* pVal);
int AtomicAdd(int* pVal, int add);

_NAMESPACE_END

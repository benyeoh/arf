//==============================================================================
//
//		AtomicFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/07/10
//
//==============================================================================

#include "Core.h"

_NAMESPACE_BEGIN

#ifdef _WIN32

#include "windows.h"

void* AtomicCompareAndSwapPtr(void** pDest, void* exchangeVal, void* compareVal)
{
	return InterlockedCompareExchangePointer(pDest, exchangeVal, compareVal);
}

int AtomicCompareAndSwap(int* pDest, int exchangeVal, int compareVal)
{
	return InterlockedCompareExchange((LONG*)pDest, exchangeVal, compareVal);
}

int	AtomicSwap(int* pDest, int exchangeVal)
{
	return InterlockedExchange((LONG*)pDest, exchangeVal);
}

int AtomicInc(int* pVal)
{
	return InterlockedIncrement((LONG*)pVal);
}

int AtomicDec(int* pVal)
{
	return InterlockedDecrement((LONG*)pVal);
}

int AtomicAdd(int* pVal, int add)
{
	return InterlockedExchangeAdd((LONG*)pVal, add) + add;
}

#else

void* AtomicCompareAndSwapPtr(void** pDest, void* exchangeVal, void* compareVal)
{
	_DEBUG_ASSERT(FALSE);
	return NULL;
}

int
AtomicCompareAndSwap(int* pDest, int exchangeVal, int compareVal)
{
	_DEBUG_ASSERT(FALSE);
	return 0;
}

int	AtomicSwap(int* pDest, int exchangeVal)
{
	_DEBUG_ASSERT(FALSE);
	return 0;
}

int 
AtomicInc(int* pVal)
{
	_DEBUG_ASSERT(FALSE);
	return 0;
}

int AtomicDec(int* pVal)
{
	_DEBUG_ASSERT(FALSE);
	return 0;
}

int AtomicAdd(int* pVal, int add)
{
	_DEBUG_ASSERT(FALSE);
	return 0;
}

#endif

_NAMESPACE_END

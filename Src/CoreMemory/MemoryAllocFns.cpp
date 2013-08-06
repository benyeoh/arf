//==============================================================================
//
//		MemoryAllocFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/13/2008
//
//==============================================================================

#include "CoreMemoryFramework.h"

_NAMESPACE_BEGIN

void* 
CoreMemoryAlloc(size_t size, const wchar* pFile, uint lineNum)
{
	if(size > 0)
		return g_pSharedAllocator->AllocChunk(size, pFile, lineNum);
		
	return NULL;
}

void 
CoreMemoryFree(void* p)
{
	if(p)
		g_pSharedAllocator->FreeChunk(p);
}

void* 
CoreMemoryRealloc(void* p, size_t size, const wchar* pFile, uint lineNum)
{
	void* pDest = NULL;
	if(p)
	{	
		if(size > 0)
		{
			pDest = g_pSharedAllocator->AllocChunk(size, pFile, lineNum);
			memcpy(pDest, p, size);
		}
		
		g_pSharedAllocator->FreeChunk(p);
	}
	else
	{
		pDest = g_pSharedAllocator->AllocChunk(size, pFile, lineNum);
	}
		
	return pDest;
}

void* 
CoreMemoryCalloc(uint count, size_t size, const wchar* pFile, uint lineNum)
{
	if(count == 0 || size == 0)
		return NULL;
		
	void* pDest = g_pSharedAllocator->AllocChunk(count * size, pFile, lineNum);
	memset(pDest, 0, count * size);

	return pDest;
}

_NAMESPACE_END

_USE_NAMESPACE

const CMMetrics& 
GetCoreMemoryMetrics()
{
	return g_pSharedAllocator->GetMetrics();
}

CMAllocFns
GetCoreMemoryBindings()
{
	CMAllocFns toReturn;
	toReturn.allocFn = &CoreMemoryAlloc;
	toReturn.freeFn = &CoreMemoryFree;
	toReturn.reallocFn = &CoreMemoryRealloc;
	toReturn.callocFn = &CoreMemoryCalloc;
	
	return toReturn;
}


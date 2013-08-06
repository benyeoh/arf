//================================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		14/9/2011
//
//================================================================================

#pragma once 

#define _DECL_CORE_MEM_ALLOC_FNS(pMemAllocator) \
 \
void* CoreMemoryAlloc(size_t size, const wchar* pFile, uint lineNum) \
{ \
	if(size > 0) \
		return pMemAllocator->Alloc(size, pFile, lineNum); \
 \
	return NULL; \
} \
 \
void CoreMemoryFree(void* p) \
{ \
	if(p) \
		pMemAllocator->Free(p); \
} \
 \
void* CoreMemoryRealloc(void* p, size_t size, const wchar* pFile, uint lineNum) \
{ \
	void* pDest = NULL; \
	if(p) \
	{ \
		if(size > 0) \
		{ \
			pDest = pMemAllocator->Alloc(size, pFile, lineNum); \
			memcpy(pDest, p, size); \
		} \
 \
		pMemAllocator->Free(p); \
	} \
	else \
	{ \
		pDest = pMemAllocator->Alloc(size, pFile, lineNum); \
	} \
 \
	return pDest; \
} \
 \
void* CoreMemoryCalloc(uint count, size_t size, const wchar* pFile, uint lineNum) \
{ \
	if(count == 0 || size == 0) \
		return NULL; \
 \
	void* pDest = pMemAllocator->Alloc(count * size, pFile, lineNum); \
	memset(pDest, 0, count * size); \
 \
	return pDest; \
} \
 \
MemAllocFns GetCoreMemoryBindings() \
{ \
	MemAllocFns toReturn; \
	toReturn.allocFn = &CoreMemoryAlloc; \
	toReturn.freeFn = &CoreMemoryFree; \
	toReturn.reallocFn = &CoreMemoryRealloc; \
	toReturn.callocFn = &CoreMemoryCalloc; \
 \
	return toReturn; \
}
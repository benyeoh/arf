//==============================================================================
//
//		MemoryBindings.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/6/2008
//
//==============================================================================

#include "Core.h"

#undef malloc
#undef realloc
#undef free
#undef calloc

#include <malloc.h>

_NAMESPACE_BEGIN

void*	
DefaultMemoryAlloc(size_t size, const wchar* pFile, uint lineNum)
{
	return ::malloc(size);
}

void	
DefaultMemoryFree(void* p)
{
	::free(p);	
}

void*	
DefaultMemoryRealloc(void* p, size_t size, const wchar* pFile, uint lineNum)
{
	return ::realloc(p, size);
}

void*	
DefaultMemoryCalloc(uint count, size_t size, const wchar* pFile, uint lineNum)
{
	return ::calloc(count, size);
}

_NAMESPACE_END

MemoryAlloc		g_MemoryAllocFn		= & _NAMESPACE DefaultMemoryAlloc;
MemoryFree		g_MemoryFreeFn		= & _NAMESPACE DefaultMemoryFree;
MemoryRealloc	g_MemoryReallocFn	= &	_NAMESPACE DefaultMemoryRealloc;
MemoryCalloc	g_MemoryCallocFn	= &	_NAMESPACE DefaultMemoryCalloc;

_USE_NAMESPACE

void* 
AlignedMalloc(uint alignment, size_t size, const wchar* pFile, uint lineNum)
{
	_DEBUG_ASSERT(alignment > 8);
	_DEBUG_ASSERT(IsPowerOf2(alignment));

	size_t alignMask = alignment - 1;

	byte* p = (byte*) (*g_MemoryAllocFn)(size + alignment, pFile, lineNum);
	_DEBUG_ASSERT(p);
	
	size_t alignOffset = (alignMask & (size_t) p);
	byte* pToReturn = ((byte*) p) - alignOffset + alignment;
	
	*(uint*)(pToReturn - sizeof(uint)) = (uint)(pToReturn - p);

	_DEBUG_ASSERT(IsAlignedToXBytes(pToReturn, alignment));
	return pToReturn;
}

void 
AlignedFree(void* p)
{
	uint offset = *(((uint*)p) - 1);
	byte* pToFree = ((byte*)p) - offset;
	
	_FREE(pToFree);
}
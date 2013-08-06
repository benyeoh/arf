//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/8/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct CMMetrics
{
	uint numPagesFree;
	uint numPageAllocs;
	uint numChunkAllocs;
	uint totalPageSizeUsed;
	uint totalChunkSizeUsed;
	uint totalHeapSizeUsed;
};

struct ICMMemAllocator : IRefCounted
{
	_PURE( const CMMetrics& GetMetrics() )
	_PURE( void	DumpLeaks() )
	_PURE( void* Alloc(size_t size, const wchar* pFile, uint lineNum) )
	_PURE( void Free(void* pChunk) )
};


//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ICMMemAllocator)

_NAMESPACE_END

extern "C"
{
	typedef _NAMESPACE ICMMemAllocator* (*CreateMemAllocatorFn)();
	_DECL_DYNAMIC_EXPORT(_NAMESPACE ICMMemAllocator* CreateMemAllocator())
}


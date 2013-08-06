//==============================================================================
//
//		MemoryBindings.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/6/2008
//
//==============================================================================

#pragma once 

typedef void*	(*MemoryAlloc)(size_t size, const wchar* pFile, uint lineNum);
typedef void	(*MemoryFree)(void* p);
typedef void*	(*MemoryRealloc)(void* p, size_t size, const wchar* pFile, uint lineNum);
typedef void*	(*MemoryCalloc)(uint count, size_t size, const wchar* pFile, uint lineNum);

extern MemoryAlloc		g_MemoryAllocFn;
extern MemoryFree		g_MemoryFreeFn;
extern MemoryRealloc	g_MemoryReallocFn;
extern MemoryCalloc		g_MemoryCallocFn;

#define _MALLOC(size)			(*g_MemoryAllocFn)(size, __WFILE__, __LINE__)
#define _REALLOC(p, size)		(*g_MemoryReallocFn)(p, size, __WFILE__, __LINE__)
#define _CALLOC(count, size)	(*g_MemoryCallocFn)(count, size, __WFILE__, __LINE__)
#define _FREE(p)				(*g_MemoryFreeFn)(p)

#undef malloc
#undef realloc
#undef calloc
#undef free

//#define malloc(size)			_MALLOC(size)
//#define realloc(p, size)		_REALLOC(p, size)
//#define calloc(count, size)		_CALLOC(count, size)
//#define free(p)					_FREE(p)

inline void* operator new(size_t size)	{	return (*g_MemoryAllocFn)(size, _W("new"), 0); }
inline void* operator new[](size_t size)	{	return (*g_MemoryAllocFn)(size, _W("new[]"), 0); }

inline void* operator new(size_t size, const wchar* pFile, uint lineNum)	{ return (*g_MemoryAllocFn)(size, pFile, lineNum); }
inline void* operator new[](size_t size, const wchar* pFile, uint lineNum)	{ return (*g_MemoryAllocFn)(size, pFile, lineNum); }

inline void operator delete(void* p)									{ (*g_MemoryFreeFn)(p); }
inline void operator delete(void* p, const wchar* pFile, uint lineNum)	{ (*g_MemoryFreeFn)(p); }

inline void operator delete[](void* p)										{ (*g_MemoryFreeFn)(p); }
inline void operator delete[](void* p, const wchar* pFile, uint lineNum)	{ (*g_MemoryFreeFn)(p); }

#define _NEW	new(__WFILE__, __LINE__)

void* AlignedMalloc(uint alignment, size_t size, const wchar* pFile, uint lineNum);
void AlignedFree(void* p);

#define _ALIGNED_MALLOC(align, size)	AlignedMalloc(align, size, __WFILE__, __LINE__)
#define _ALIGNED_FREE(p)				AlignedFree(p)

#define _ALIGNED_NEW(align, type)		new(_ALIGNED_MALLOC(align, sizeof(type))) 
#define _ALIGNED_DELETE(p, type)		{ if((p)) { (p)->~type(); _ALIGNED_FREE((p)); (p)=0;} }

#define _IMPL_ALIGNED_ALLOCS(align) \
	void* operator new[](size_t size) { return AlignedMalloc(align, size, __WFILE__, __LINE__); } \
	void* operator new[](size_t size, const wchar* pFile, uint lineNum) { return AlignedMalloc(align, size, pFile, lineNum); } \
	void* operator new(size_t size) { return AlignedMalloc(align, size, __WFILE__, __LINE__); } \
	void* operator new(size_t size, const wchar* pFile, uint lineNum) { return AlignedMalloc(align, size, pFile, lineNum); } \
	void operator delete(void* p) {	AlignedFree(p); } \
	void operator delete(void* p, const wchar* pFile, uint lineNum) { AlignedFree(p); } \
	void operator delete[](void* p) {	AlignedFree(p); } \
	void operator delete[](void* p, const wchar* pFile, uint lineNum) { AlignedFree(p); }

struct MemAllocFns
{
	MemoryAlloc allocFn;
	MemoryFree freeFn;
	MemoryRealloc reallocFn;
	MemoryCalloc callocFn;	
};

#define _BIND_TO_GLOBAL_ALLOC(allocFnsStruct) \
	g_MemoryAllocFn = allocFnsStruct.allocFn; \
	g_MemoryFreeFn	= allocFnsStruct.freeFn; \
	g_MemoryReallocFn = allocFnsStruct.reallocFn; \
	g_MemoryCallocFn = allocFnsStruct.callocFn;

#define _BIND_TO_ALLOC_STRUCT(allocFnsStruct) \
	allocFnsStruct.allocFn = g_MemoryAllocFn; \
	allocFnsStruct.freeFn =  g_MemoryFreeFn; \
	allocFnsStruct.reallocFn = g_MemoryReallocFn; \
	allocFnsStruct.callocFn = g_MemoryCallocFn;


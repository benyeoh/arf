//================================================================================
//
//		Module.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/9/2011
//
//================================================================================

#pragma once

typedef size_t Module;

// Move these to another platform specific library
#ifdef _WIN32
	#define _DECL_DYNAMIC_EXPORT(fn) __declspec(dllexport) fn;
#else
	#define _DECL_DYNAMIC_EXPORT(fn) fn;
#endif

_NAMESPACE_BEGIN

Module	InitializeModule(const wchar* pModName, MemAllocFns* pAllocFns);
void*	GetFunction(Module mod, const char* pProcName);
void	ShutdownModule(Module module);

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//
//	Mem Alloc Bindings
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef void (*SetMemAllocFn)(MemAllocFns fns);
	_DECL_DYNAMIC_EXPORT( void SetMemAlloc(MemAllocFns fns) )
}

inline void SetMemAlloc(MemAllocFns fns)
{
	_BIND_TO_GLOBAL_ALLOC(fns);
}

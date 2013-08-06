//================================================================================
//
//		Module.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/9/2011
//
//================================================================================

#include "Core.h"

#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#endif

_NAMESPACE_BEGIN

#ifdef _WIN32
	
Module	InitializeModule(const wchar* pModName, MemAllocFns* pAllocFns)
{
	// Get the current HMODULE of this function
	HMODULE hThisMod = NULL;
	MEMORY_BASIC_INFORMATION info;
	::VirtualQuery(InitializeModule, &info, sizeof(info));
	hThisMod = (HMODULE) info.AllocationBase;

	// Set the directory of this module as one of the search paths
	wchar fileName[256];
	wchar savedDir[256];
	::GetModuleFileName(hThisMod, fileName, sizeof(fileName));
	size_t strLen = wcslen(fileName);
	for(size_t i=strLen-2; i >=0; i--)
	{
		if(fileName[i] == _W('\\'))
		{
			fileName[i] = 0;
			break;
		}
	}

	::GetCurrentDirectory(sizeof(savedDir), savedDir);
	::SetCurrentDirectory(fileName);

	// Load module and automatically set the allocator fns
	HMODULE hMod = ::LoadLibrary(pModName);
	
	// Reset directory
	::SetCurrentDirectory(savedDir);

	SetMemAllocFn CallSetMemAlloc = (SetMemAllocFn) ::GetProcAddress(hMod, "SetMemAlloc");

	// Set memory alloc for module
	if(CallSetMemAlloc && pAllocFns)
		CallSetMemAlloc(*pAllocFns);

	return (Module) hMod;
}

void	ShutdownModule(Module module)
{
	::FreeLibrary((HMODULE) module);
}

void*	GetFunction(Module mod, const char* pFnName)
{
	void* pFn = (void*) ::GetProcAddress((HMODULE) mod, pFnName);
	return pFn;
}

#else

Module	InitializeModule(const wchar* pModName, MemAllocFns* pAllocFns)
{
	return 0;
}

void	ShutdownModule(Module module)
{
}

void*	GetFunction(Module mod, const wchar* pFnName)
{
	return 0;
}

#endif

_NAMESPACE_END

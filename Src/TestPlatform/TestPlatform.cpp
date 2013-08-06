// TestPlatform.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

IPPlatformPtr g_pPlatform;

ICMMemAllocatorPtr g_pAllocator;
_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

void Run()
{
	Module hCoreMem		= InitializeModule(_W("CoreMemory.dll"), NULL);

	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);

	Module hPlatform = InitializeModule(_W("PlatformWin32.dll"), &allocFns);

	CreatePlatformFn CreatePlatform = (CreatePlatformFn) GetFunction(hPlatform, "CreatePlatform");
	g_pPlatform = CreatePlatform();

	RunThreads();
	RunThreadPool();

	g_pPlatform = NULL;

	ShutdownModule(hPlatform);

	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;

	ShutdownModule(hCoreMem);

}

int _tmain(int argc, _TCHAR* argv[])
{
	Run();

	return 0;
}


//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#include "FileSystemWin32.h"

_NAMESPACE_BEGIN

CFFileSystem* g_pFileSystem = NULL;

_NAMESPACE_END

_USE_NAMESPACE

// HMODULE g_hCoreMem = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//if(!g_hCoreMem)
		//{
		//	g_hCoreMem = ::LoadLibrary(_W("CoreMemory.dll"));

		//	GetCoreMemoryBindingsFn GetCoreMemBindings = (GetCoreMemoryBindingsFn) ::GetProcAddress(g_hCoreMem, "GetCoreMemoryBindings");
		//	MemAllocFns allocFns = GetCoreMemBindings();
		//	_BIND_TO_GLOBAL_ALLOC(allocFns);
		//}
		break;

	case DLL_THREAD_ATTACH:	break;
	case DLL_THREAD_DETACH:	break;
	case DLL_PROCESS_DETACH:
		{
			//if(g_hCoreMem)
			//{
			//	::FreeLibrary(g_hCoreMem);
			//	g_hCoreMem = NULL;
			//}	
			break;
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CreateFileSystem()
//		
//------------------------------------------------------------------------------
IFFileSystem* CreateFileSystem()
{
	if(g_pFileSystem == NULL)
		g_pFileSystem = _NEW CFFileSystemWin32;

	return g_pFileSystem;
}


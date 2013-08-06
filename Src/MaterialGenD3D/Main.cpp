//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenD3D.h"

_USE_NAMESPACE

HMODULE g_hCoreMem = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			//if(!g_hCoreMem)
			//{
			//	g_hCoreMem = ::LoadLibrary(_W("CoreMemory.dll"));

			//	GetCoreMemoryBindingsFn GetCoreMemBindings = (GetCoreMemoryBindingsFn) ::GetProcAddress(g_hCoreMem, "GetCoreMemoryBindings");
			//	MemAllocFns allocFns = GetCoreMemBindings();
			//	_BIND_TO_GLOBAL_ALLOC(allocFns);
			//}
			break;
		}
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


IMGMaterialGen* 
CreateMaterialGen()
{	
	return _NEW CMGMaterialGenD3D();
}

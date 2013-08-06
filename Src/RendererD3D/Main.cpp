//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, February 07, 2006
//
//==============================================================================

#include "RendererD3D.h"

_USE_NAMESPACE

//HMODULE g_hCoreMem = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: break;
		//{
		//	if(!g_hCoreMem)
		//	{
		//		g_hCoreMem = ::LoadLibrary(_W("CoreMemory.dll"));

		//		GetCoreMemoryBindingsFn GetCoreMemBindings = (GetCoreMemoryBindingsFn) ::GetProcAddress(g_hCoreMem, "GetCoreMemoryBindings");
		//		MemAllocFns allocFns = GetCoreMemBindings();
		//		_BIND_TO_GLOBAL_ALLOC(allocFns);
		//	}
		//	break;
		//}
	case DLL_THREAD_ATTACH:	break;
	case DLL_THREAD_DETACH:	break;
	case DLL_PROCESS_DETACH: break;
		//{
		//	if(g_hCoreMem)
		//	{
		//		::FreeLibrary(g_hCoreMem);
		//		g_hCoreMem = NULL;
		//	}	
		//	break;
		//}
	}
    return TRUE;
}


//------------------------------------------------------------------------------
//	CreateRenderer()
//		
//------------------------------------------------------------------------------
IRRenderer* 
CreateRenderer()
{	
	return _NEW CRRendererD3D;
}

void*
GetD3DDevice(IRRenderer* pRendererD3D)
{
	return ((CRRendererD3D*)pRendererD3D)->GetD3DDevice();
}

//void SetMemAlloc(MemAllocFns fns)
//{
//	_BIND_TO_GLOBAL_ALLOC(fns);
//}
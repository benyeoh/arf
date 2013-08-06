//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/30/2008
//
//==============================================================================

#include "CoreMemoryFramework.h"

_USE_NAMESPACE

//CMSegregatedListAllocator* _NAMESPACE g_pSharedAllocator = NULL;

#if defined(_MSC_VER) && defined(_WINDOWS) && defined(_USRDLL)

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			//if(!g_pSharedAllocator)
			//	g_pSharedAllocator = new CMSegregatedListAllocator;	
			//g_pSharedAllocator->AddRef();
			break;
		}
		
		case DLL_THREAD_ATTACH:	break;
		case DLL_THREAD_DETACH:	break;
		
		case DLL_PROCESS_DETACH:
		{
			//_DEBUG_ASSERT(g_pSharedAllocator);
			//
			//int refCount = g_pSharedAllocator->Release();
			//if(refCount == 0)
			//	g_pSharedAllocator = NULL;	
			break;
		}	
	}

	return TRUE;
}

#endif

ICMMemAllocator* CreateMemAllocator()
{
	return new CMSegregatedListAllocator;
}


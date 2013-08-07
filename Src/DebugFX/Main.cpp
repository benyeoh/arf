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

#include "DebugFXFramework.h"

_USE_NAMESPACE

#if defined(_MSC_VER) && defined(_WINDOWS) && defined(_USRDLL)

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

#endif

//------------------------------------------------------------------------------
//	CreateDebugFX()
//
//------------------------------------------------------------------------------
IDFXDebugFX* 
CreateDebugFX()
{
	return _NEW CDFXDebugFX;
}

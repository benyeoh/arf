//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/09/2009
//
//==============================================================================

#include "AmbientFXFramework.h"

_USE_NAMESPACE

#if defined(_WINDLL)

#include "windows.h"

//HMODULE g_hCoreMem = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	return TRUE;
}

#endif

IAFXAmbientFX* CreateAmbientFX()
{	
	return _NEW CAFXAmbientFX;
}

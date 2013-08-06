//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/30/2007
//
//==============================================================================

#include "FontFXFramework.h"

_USE_NAMESPACE

#if defined(_WINDLL) 

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
//	CreateFontFX()
//
//------------------------------------------------------------------------------
IFFXFontFX* 
CreateFontFX()
{
	return _NEW CFFXFontFX;
}

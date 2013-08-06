//================================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/14/2012
//
//================================================================================

#include "SHPRTComputeFramework.h"

_USE_NAMESPACE

#if defined(_WINDLL)

#include "windows.h"

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

#endif

ISHPRTCompute* CreateSHPRTCompute()
{	
	return _NEW CSHPRTCompute;
}

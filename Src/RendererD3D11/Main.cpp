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

#include "RendererD3D11Framework.h"

_USE_NAMESPACE

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: break;
	case DLL_THREAD_ATTACH:	break;
	case DLL_THREAD_DETACH:	break;
	case DLL_PROCESS_DETACH: break;
	}
	return TRUE;
}


//------------------------------------------------------------------------------
//	CreateRenderer()
//		
//------------------------------------------------------------------------------
IRRenderer* CreateRenderer()
{	
	return _NEW CRRendererD3D11;
}

//void* GetD3DDevice(IRRenderer* pRendererD3D)
//{
//	return ((CRRendererD3D*)pRendererD3D)->GetD3DDevice();
//}

//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			AllocateGlobalData();
			break;
		}
		
		case DLL_PROCESS_DETACH:
		{
			FreeGlobalData();
			break;
		}
	}
	
    return TRUE;
}

XSIPLUGINCALLBACK XSI::CStatus 
XSILoadPlugin(XSI::PluginRegistrar& in_reg)
{
	Initialize();

	in_reg.PutAuthor(_W("Ben Yeoh"));
	in_reg.PutName(_W("ARF Renderer Plugin"));
	in_reg.PutVersion(1, 0);
	
	RegisterGetRendererCommand(in_reg);
	RegisterExportMeshDataCommand(in_reg);
	//RegisterRendererMenu(in_reg);
	//RegisterExportMeshDataProperty(in_reg);
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus 
XSIUnloadPlugin(XSI::PluginRegistrar& in_reg)
{
	Shutdown();
	
	return XSI::CStatus::OK;
}


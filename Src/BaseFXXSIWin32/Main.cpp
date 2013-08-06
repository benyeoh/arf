//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/9/2008
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
	in_reg.PutName(_W("ARF BaseFX Plugin"));
	in_reg.PutVersion(1, 0);

	RegisterBaseFXMenu(in_reg);
	RegisterCreateBaseFXMaterialProperty(in_reg);
	RegisterExportBFXUMeshListProperty(in_reg);
	RegisterExportBFXMaterialCommand(in_reg);
	RegisterExportBFXMeshCommand(in_reg);
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus 
XSIUnloadPlugin(XSI::PluginRegistrar& in_reg)
{
	Shutdown();

	return XSI::CStatus::OK;
}


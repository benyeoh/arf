//================================================================================
//
//		FileSystemMenu.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		21/10/2011
//
//================================================================================

#include "stdafx.h"

XSI::CStatus
RegisterFileSystemMenu(XSI::PluginRegistrar& in_reg)
{
	XSI::PluginItem menu = in_reg.RegisterMenu(XSI::siMenuMainFileProjectID, _W("FileSystem"), false, false);
	//menu = in_reg.RegisterMenu(XSI::siMenuMainFileExportID, _W("ExportBFXUMeshList"), false, false);

	return XSI::CStatus::OK;
	//XSI::PluginItem menu2 = in_reg.RegisterMenu(XSI::siMenuMainTopLevelID, _THE_PROJECT_LABEL, false, false);
	//
	//if(menu == menu2)
	//{
	//	XSI::Application app;
	//	app.LogMessage(_W("Cheebye!!!"), XSI::siFatalMsg);
	//}
}

XSIPLUGINCALLBACK 
XSI::CStatus FileSystem_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		

	XSI::MenuItem menuItem;
	oMenu.AddCallbackItem(_W("ARF File System Settings"), _W("OnFileSystemSettingsClicked"), menuItem);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK
XSI::CStatus OnFileSystemSettingsClicked(XSI::CRef& in_ref)
{
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	XSI::CustomProperty prop = root.GetProperties().GetItem(_W("FSPropSelectPath"));
	
	if(!prop.IsValid())
	{
		prop = root.AddProperty(_W("FSPropSelectPath"));
	}

	ShowProperty(prop, _W("FSPropSelectPath"));
	
	return XSI::CStatus::OK;
}

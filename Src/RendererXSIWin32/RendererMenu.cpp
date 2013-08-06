//==============================================================================
//
//		RendererMenu.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/12/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterRendererMenu(XSI::PluginRegistrar& in_reg)
{
	//XSI::PluginItem menu = in_reg.RegisterMenu(XSI::siMenuMainTopLevelID, _W("ARF Renderer"), false, false);

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
XSI::CStatus ARFRenderer_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		

	XSI::MenuItem menuItem;
	oMenu.AddCallbackItem(_W("Export Mesh Data"), _W("OnExportMeshDataClicked"), menuItem);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK
XSI::CStatus OnExportMeshDataClicked(XSI::CRef& in_ref)
{
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	XSI::CustomProperty prop = root.GetProperties().GetItem(_W("Export_Mesh_Data"));
	XSI::Scene scene = app.GetActiveProject().GetActiveScene();

	if(!prop.IsValid())
	{
		prop = root.AddProperty(_W("RPropExportMeshData"));
		prop.PutName(_W("Export_Mesh_Data"));
	}
	
	ShowProperty(prop, _W("Export Mesh Data Property"), XSI::siFollow);
	
	//XSI::Application app;

	//ShowProperty(_W("RPropExportMeshData"), _W("Export Mesh Data"));

	return XSI::CStatus::OK;
}
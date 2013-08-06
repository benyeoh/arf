//==============================================================================
//
//		ExportMenu.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/16/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterExportMenu(XSI::PluginRegistrar& in_reg)
{
	XSI::PluginItem menu = in_reg.RegisterMenu(XSI::siMenuMainFileExportID, _W("ExportSceneObject"), false, false);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK 
XSI::CStatus ExportSceneObject_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		

	XSI::MenuItem menuItem;
	oMenu.AddCallbackItem(_W("ARF Scene Object"), _W("OnExportSceneObjectClicked"), menuItem);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK
XSI::CStatus OnExportSceneObjectClicked(XSI::CRef& in_ref)
{
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	XSI::CustomProperty prop = root.GetProperties().GetItem(_W("AppPropExportSceneObject"));
	
	if(!prop.IsValid())
	{
		prop = root.AddProperty(_W("AppPropExportSceneObject"));
	}

	ShowProperty(prop, _W("Export Scene Object"));

	return XSI::CStatus::OK;
}
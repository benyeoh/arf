//================================================================================
//
//		SelectPathProperty.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		21/10/2011
//
//================================================================================

#include "stdafx.h"

XSI::CStatus
RegisterSelectPathProperty(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterProperty(_W("FSPropSelectPath"));
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
FSPropSelectPath_Define(XSI::CRef& in_ctxt)
{
	InitializeFileSystem();
	
	XSI::Context ctxt(in_ctxt);

	XSI::CustomProperty prop = ctxt.GetSource();
	XSI::Parameter param;

	prop.AddParameter(_W("FSParamSelectPathShdDir"), 
		XSI::CValue::siString, 
		XSI::siPersistable,
		_W("Shaders Directory"),
		_W("The normalized output directory path to the shaders directory"),
		XSI::CString(g_pFileSystem->GetResourcePath()) + _W("\\") + DEF_SHADERS_PATH,
		param);

	return XSI::CStatus::OK;	
}

XSIPLUGINCALLBACK XSI::CStatus
FSPropSelectPath_UpdateLayout(XSI::CRef& in_ctxt)
{
	InitializeFileSystem();
	
	XSI::Context ctxt(in_ctxt);

	XSI::PPGLayout layout = ctxt.GetSource();
	layout.Clear() ;

	XSI::PPGItem item;

	layout.AddGroup(_W("File System Directory Paths"));
	item = layout.AddItem(_W("FSParamSelectPathShdDir"), _W("Shaders Directory"), XSI::siControlFolder);
	item.PutAttribute(XSI::siUIInitialDir, XSI::CString(g_pFileSystem->GetResourcePath()) + _W("\\") + DEF_SHADERS_PATH);
	//item.PutAttribute(XSI::siUISubFolder, _W("data\\meshes"));

	//item.PutAttribute(XSI::siUICX, 365L);

//	item = layout.AddItem(_W("RParamRequireTangents"), _W("Require tangent and binormals?"), XSI::siControlCheck);
	//item.PutAttribute(XSI::siUICX, 365L);

	layout.EndGroup();
	layout.AddSpacer(1, 2);
	item = layout.AddButton(_W("BFXButtonSelectPathUpdate"), _W("Update"));
	item.PutAttribute(XSI::siUICX, 365L);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
FSPropSelectPath_DefineLayout(XSI::CRef& in_ctxt)
{
	return FSPropSelectPath_UpdateLayout(in_ctxt);
}

XSIPLUGINCALLBACK XSI::CStatus
FSPropSelectPath_PPGEvent(XSI::CRef& in_ctxt)
{
	XSI::PPGEventContext ctxt(in_ctxt);
	XSI::PPGEventContext::PPGEvent eventID = ctxt.GetEventID();

	XSI::Application app;
	//XSI::UIToolkit kit = app.GetUIToolkit();
	XSI::CStatus status = XSI::CStatus::OK;

	if(eventID == XSI::PPGEventContext::siOnInit)
	{
		// This event meant that the UI was just created.
		// It gives us a chance to set some parameter values.
		// We could even change the layout completely at this point.

		// For this event Source() of the event is the CustomProperty object

		XSI::CustomProperty prop = ctxt.GetSource() ;

		app.LogMessage( L"OnInit called for " + prop.GetFullName() ) ;

		// If you regenerate the layout then call this:
		//ctxt.PutAttribute(L"Refresh", true);

	}
	else 
	if(eventID == XSI::PPGEventContext::siButtonClicked)
	{
		// If there are multiple buttons 
		// we can use this attribute to figure out which one was clicked.
		XSI::CValue buttonPressed = ctxt.GetAttribute( L"Button" ) ;
		app.LogMessage( L"Button pressed: " + buttonPressed.GetAsText() ) ;

		if(buttonPressed.GetAsText() == _W("BFXButtonSelectPathUpdate"))
		{
			XSI::CustomProperty prop = ctxt.GetSource();
			XSI::CString path = prop.GetParameterValue(_W("FSParamSelectPathShdDir"));
			wchar normPath[256];
						
			g_pFileSystem->ClearMnemonics();
			
			g_pFileSystem->NormalizePath(path.GetWideString(), normPath, sizeof(normPath)-1);
			g_pFileSystem->AddMnemonic(_W("shd"), normPath);

			XSI::CString pathLog = _W("[shd]: ");
			pathLog = pathLog + normPath;
			app.LogMessage(pathLog.GetWideString());

			// OHOHOHOHO: NEED TO JUST PARSE ALL EFFECTS IN THE FOLDER AND SUBS
			// For now, just re-add these
			g_pFileSystem->AddMnemonic(_W("msh"), DEF_MESHES_PATH);
			g_pFileSystem->AddMnemonic(_W("tex"), DEF_TEXTURES_PATH);
			g_pFileSystem->AddMnemonic(_W("mat"), DEF_MATERIALS_PATH);
			g_pFileSystem->AddMnemonic(_W("dat"), DEF_DATA_PATH);
		}
	}
	else 
	if(eventID == XSI::PPGEventContext::siTabChange)
	{
		// We will be called when the PPG is first opened
		// and every time the tab changes

		// Retrieve the label of the tab that is now active
		XSI::CValue tabLabel = ctxt.GetAttribute( L"Tab" ) ;

		app.LogMessage( L"Tab changed to: " + tabLabel .GetAsText() ) ;
	}
	else 
	if(eventID == XSI::PPGEventContext::siParameterChange)
	{
		// For this event the Source of the event is the parameter
		// itself
		XSI::Parameter changed = ctxt.GetSource() ;
		XSI::CustomProperty prop = changed.GetParent() ;
		XSI::CString   paramName = changed.GetScriptName() ; 

		app.LogMessage( L"Parameter Changed: " + paramName ) ;
	}

	return status;
}

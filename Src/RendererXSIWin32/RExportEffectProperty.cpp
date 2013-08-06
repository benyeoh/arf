//==============================================================================
//
//		RExportEffectProperty.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/11/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterRExportEffectProperty(XSI::PluginRegistrar& in_reg)
{
	//in_reg.RegisterProperty(_W("RPropExportMeshData"));
	in_reg.RegisterProperty(_W("RPropExportEffect"));

	return XSI::CStatus::OK;
}


std::vector<IREffectPtr>&
GetEffects()
{
	static std::vector<IREffectPtr> s_Effects;

	InitializeRenderer();

	_DEBUG_ASSERT(g_pRenderer && g_pFileSystem);

	s_Effects.clear();
	IFFileEnumeratorPtr pFileEnum = g_pFileSystem->GetFiles( _W("[shd]\\*.fx") );

	IFFilePtr pFile = pFileEnum->GetNextFile();

	while(pFile)
	{
		const wchar* pFileName = pFile->GetFileName();
		IREffect* pEffect = g_pRenderer->GetRResourceMgr().LoadREffect(pFileName, NULL, NULL, 0);

		if(pEffect)
		{
			s_Effects.push_back(pEffect);
		}
	}
	
	return s_Effects;
}

XSI::CStatus
RPropExportEffect_DefineConstant(XSI::CustomProperty& prop)
{
	XSI::Parameter param;
	
	prop.AddParameter(_W("RParamEffectCombo"), 
		XSI::CValue::siInt4, 
		XSI::siPersistable,
		_W(""),
		_W("The list of effects"),
		0,
		param);

	prop.PutParameterValue(_W("RParamEffectCombo"), 0);
	
	//prop.AddParameter(_W("RParamRequireTangents"), 
	//	XSI::CValue::siBool, 
	//	XSI::siPersistable,
	//	_W("Require tangent and binormals?"),
	//	_W("Select to export with tangent the binormal data for each vertex"),
	//	true,
	//	param);

}

XSI::CStatus
RPropExportEffect_DefineDynamic(XSI::CustomProperty& prop)
{
}


XSIPLUGINCALLBACK XSI::CStatus
RPropExportEffect_Define(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);

	XSI::CustomProperty prop = ctxt.GetSource();
	XSI::Parameter param;

	XSI::CStatus statusConstant = RPropExportEffect_DefineConstant(prop);
	XSI::CStatus statusDynamic = RPropExportEffect_DefineDynamic(prop);

	if( statusConstant == XSI::CStatus::OK &&
		statusDynamic == XSI::CStatus::OK )
	{
		return XSI::CStatus::OK;
	}

	return XSI::CStatus::Fail;	
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportEffect_UpdateLayout(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);

	XSI::PPGLayout layout = ctxt.GetSource();
	layout.Clear() ;

	XSI::PPGItem item;

	// Get all effects
	std::vector<IREffectPtr>& effects = GetEffects();
	XSI::CValueArray effectComboItems;
	uint numEffects = effects.size();
	_LOOPi(numEffects)
	{
		XSI::CString effectDesc = effects[i]->GetEffectDesc()
		effectComboItems.Add()
	}

	layout.AddGroup(_W("Effect"));
	item = layout.AddEnumControl(_W("RParamEffectCombo"), )
	item = layout.AddItem(_W("RParamRequireTangents"), _W("Require tangent and binormals?"), XSI::siControlCheck);

	layout.EndGroup();
	layout.AddSpacer(1, 2);
	item = layout.AddButton(_W("RButtonExportMeshData"), _W("Export"));
	item.PutAttribute(XSI::siUICX, 365L);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportEffect_DefineLayout(XSI::CRef& in_ctxt)
{
	return RPropExportEffect_UpdateLayout(in_ctxt);
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportEffect_PPGEvent(XSI::CRef& in_ctxt)
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

		//if(buttonPressed.GetAsText() == _W("RButtonExportMeshData"))
		//{
		//	status = ExportMeshes();
		//}
	}
	else 
	if(eventID == XSI::PPGEventContext::siTabChange)
	{
		// We will be called when the PPG is first opened
		// and every time the tab changes

		// Retrieve the label of the tab that is now active
		XSI::CValue tabLabel = ctxt.GetAttribute( L"Tab" ) ;

		app.LogMessage( L"Tab changed to: " + tabLabel.GetAsText() ) ;
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

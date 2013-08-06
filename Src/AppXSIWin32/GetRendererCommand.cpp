//==============================================================================
//
//		GetRendererCommand.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterGetRendererCommand(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterCommand(_W("AppComGetRenderer"), _W("AppComGetRenderer"));

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
AppComGetRenderer_Init(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);
	//Initialize();

	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Defining: " + cmd.GetName() );

	// Returns CValue, which contains the serialized bytebuffer pointer
	cmd.EnableReturnValue(true);	

	//XSI::ArgumentArray args;
	//args = cmd.GetArguments();

	// Pointer to the material to export
	//args.Add(_W("arg0"), XSI::CRef());	

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
AppComGetRenderer_Execute(XSI::CRef& in_ctxt)
{
	// Ensure renderer is initialized
	XSI::Context ctxt(in_ctxt);
	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Executing: " + cmd.GetName() );

	InitializeRenderer();
	
	XSI::CValue toReturn((ULONG) (g_pRenderer.GetPtr()));	
	XSI::CStatus status = g_pRenderer ? XSI::CStatus::OK : XSI::CStatus::Fail;
	ctxt.PutAttribute(_W("ReturnValue"), toReturn);

	return status;		
}

XSIPLUGINCALLBACK XSI::CStatus
AppComGetRenderer_Term(XSI::CRef& in_ctxt)
{
	return XSI::CStatus::OK;
}

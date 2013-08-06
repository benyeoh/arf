//==============================================================================
//
//		GetFileSystemCommand.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterGetFileSystemCommand(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterCommand(_W("AppComGetFileSystem"), _W("AppComGetFileSystem"));

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
AppComGetFileSystem_Init(XSI::CRef& in_ctxt)
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
AppComGetFileSystem_Execute(XSI::CRef& in_ctxt)
{
	// Ensure renderer is initialized
	XSI::Context ctxt(in_ctxt);
	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Executing: " + cmd.GetName() );

	//// Unpack the command argument values
	//XSI::CValueArray args = ctxt.GetAttribute(_W("Arguments"));

	//XSI::Material mat((XSI::CRef)(args[0]));

	//// Do export mesh data
	//IByteBuffer* pMatBuffer = _NEW CByteBuffer(256);

	XSI::CValue toReturn((ULONG) (g_pFileSystem.GetPtr()));	
	XSI::CStatus status = g_pFileSystem ? XSI::CStatus::OK : XSI::CStatus::Fail;
	ctxt.PutAttribute(_W("ReturnValue"), toReturn);

	return status;		
}

XSIPLUGINCALLBACK XSI::CStatus
AppComGetFileSystem_Term(XSI::CRef& in_ctxt)
{
	return XSI::CStatus::OK;
}

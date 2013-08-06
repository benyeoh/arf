//==============================================================================
//
//		ExportRFXMeshCommand.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterExportBFXMeshCommand(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterCommand(_W("BFXComExportBFXMesh"), _W("BFXComExportBFXMesh"));

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
BFXComExportBFXMesh_Init(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);
	//Initialize();

	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Defining: " + cmd.GetName() );

	// Returns CValue, which contains the serialized bytebuffer pointer
	cmd.EnableReturnValue(true);	

	XSI::ArgumentArray args;
	args = cmd.GetArguments();

	// Pointer to the X3DObject to export
	args.Add(_W("arg0"), XSI::CRef());	

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
BFXComExportBFXMesh_Execute(XSI::CRef& in_ctxt)
{
	// Ensure renderer is initialized
	XSI::Context ctxt(in_ctxt);
	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Executing: " + cmd.GetName() );

	// Unpack the command argument values
	XSI::CValueArray args = ctxt.GetAttribute(_W("Arguments"));

	XSI::X3DObject obj((XSI::CRef)(args[0]));

	IBFXMaterialGroup* pMatGroup = NULL;
	IRVertexBufferGroup* pVBGroup = NULL;
	IRIndexBuffer* pIB = NULL;

	XSI::CStatus status = ExportBFXMesh(obj, &pVBGroup, &pIB, &pMatGroup);
	
	if(status == XSI::CStatus::Fail)
		return status;
		
	XSI::CValueArray valArray;
	valArray.Add( (ULONG) pMatGroup );
	valArray.Add( (ULONG) pVBGroup );
	valArray.Add( (ULONG) pIB );

	XSI::CValue toReturn(valArray);	
	ctxt.PutAttribute(_W("ReturnValue"), toReturn);

	return status;	
}

XSIPLUGINCALLBACK XSI::CStatus
BFXComExportBFXMesh_Term(XSI::CRef& in_ctxt)
{
	return XSI::CStatus::OK;
}
//==============================================================================
//
//		Command.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/2/2007
//
//==============================================================================

#include "stdafx.h"

_USE_NAMESPACE

XSI::CStatus
RegisterCommands(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterCommand(_W("RComExportMeshData"), _W("RComExportMeshData"));
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
RComExportMeshData_Init(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);
	//Initialize();
	
	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Defining: " + cmd.GetName() );

	cmd.EnableReturnValue(true);

	XSI::ArgumentArray args;
	args = cmd.GetArguments();

	// Declare the argument to store vertex descriptions	
	//args.Add(_W("arg0"), DEFAULT_MESH_OUT_DIR);
	//args.Add(_W("arg1"), DEFAULT_MESH_NAME);
	args.Add(_W("arg0"), (ULONG)(NULL));
	args.Add(_W("arg1"), XSI::CRef());
		
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
RComExportMeshData_Execute(XSI::CRef& in_ctxt)
{
	// Ensure renderer is initialized
	InitializeRenderer();
	
	XSI::Context ctxt(in_ctxt);
	XSI::Command cmd;
	cmd = ctxt.GetSource();

	XSI::Application app;
	app.LogMessage( L"Executing: " + cmd.GetName() );

	// Unpack the command argument values
	XSI::CValueArray args = ctxt.GetAttribute(_W("Arguments"));
	
	//XSI::CString outDir = args[0].GetAsText();
	//XSI::CString meshName = args[1].GetAsText();
	RVertexDesc* pDesc = (RVertexDesc*)((ULONG)args[0]);
	XSI::X3DObject obj((XSI::CRef)(args[1]));

	//XSI::CString vbPath = outDir + _W("\\") + meshName + _W(".vbg");	
	//XSI::CString ibPath = outDir + _W("\\") + meshName + _W(".ib");	
	
	// Do export mesh data
	IByteBuffer* pVBGroupBuffer = new CByteBuffer(256);
	IByteBuffer* pIBBuffer = new CByteBuffer(256);

	CValueArray meshData;
	meshData.Add( (ULONG) pVBGroupBuffer );
	meshData.Add( (ULONG) pIBBuffer );
	
	CValue toReturn(meshData);	
	XSI::CStatus status = ExportMesh(obj, pDesc, pVBGroupBuffer, pIBBuffer);
	ctxt.PutAttribute(_W("ReturnValue"), toReturn);
	
	return status;		
}

XSIPLUGINCALLBACK XSI::CStatus
RComExportMeshData_Term(XSI::CRef& in_ctxt)
{
	return XSI::CStatus::OK;
}
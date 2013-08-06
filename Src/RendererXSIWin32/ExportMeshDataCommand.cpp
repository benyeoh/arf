//==============================================================================
//
//		RExportMeshDataCommand.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/11/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterExportMeshDataCommand(XSI::PluginRegistrar& in_reg)
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

	// Returns CValueArray, which contains the 
	// vertex buffer and index buffer serialized data
	cmd.EnableReturnValue(true);	
	
	XSI::ArgumentArray args;
	args = cmd.GetArguments();

	// Pointer to the RVertexDesc array
	args.Add(_W("arg0"), (ULONG)(NULL));	
	// Pointer to a X3DObject to export
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
	//IByteBuffer* pVBGroupBuffer = _NEW CByteBuffer(256);
	//IByteBuffer* pIBBuffer = _NEW CByteBuffer(256);


	XSI::CValue toReturn;	
	IRVertexBufferGroup* pVBGroup = NULL;
	IRIndexBuffer* pIB = NULL;
	
	ExportMesh(obj, pDesc, &pVBGroup, &pIB);
	
	if(!pVBGroup || !pIB)
		return XSI::CStatus::Fail;
		
	XSI::CValueArray meshData;
	meshData.Add( (ULONG) pVBGroup );
	meshData.Add( (ULONG) pIB );

	toReturn = meshData;
	
	ctxt.PutAttribute(_W("ReturnValue"), toReturn);

	return XSI::CStatus::OK;		
}

XSIPLUGINCALLBACK XSI::CStatus
RComExportMeshData_Term(XSI::CRef& in_ctxt)
{
	return XSI::CStatus::OK;
}
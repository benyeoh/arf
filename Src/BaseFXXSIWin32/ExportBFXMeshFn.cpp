//==============================================================================
//
//		ExportBFXMeshFn.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/13/2008
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
ExportBFXMesh(XSI::X3DObject& obj, IRVertexBufferGroup** ppVBGroup, IRIndexBuffer** ppIB, IBFXMaterialGroup** ppMatGroup)
{
	InitializeFileSystem();
	InitializeBaseFX();
	
	XSI::Application app;
	
	XSI::CStatus status;
	
	// Export the effect group
	XSI::Material mat = obj.GetMaterial();
	XSI::CValue retVal;
	XSI::CValueArray args(1);
	args[0] = mat;
	status = app.ExecuteCommand(_W("BFXComExportBFXMaterial"), args, retVal);
	if(status == XSI::CStatus::Fail)
		return status;
		
	//IByteBufferPtr pMatBuffer = (IByteBuffer*)((ULONG)retVal);
	
	// Load the effect group
	*ppMatGroup = (IBFXMaterialGroup*)((XSI::CValue::siPtrType)retVal); //LoadEffectGroup(g_pRenderer, g_pRenderFX, pMatBuffer, 0);
	
	// Determine the vertex data that we need from the effect
	RVertexDesc desc[256];
	_LOOPi(256)
	{
		desc[i].usage		= VDU_END;		
		desc[i].usageIndex	= 0;
	}
	
	desc[0].usage = VDU_POSITION; desc[0].usageIndex = 0;
	desc[1].usage = VDU_TEXCOORDF2; desc[1].usageIndex = 0;
	desc[2].usage = VDU_NORMAL; desc[2].usageIndex = 0;
	
	AppendVertexDesc((*ppMatGroup)->GetTemplate(), desc);
	
	// Export the topology	
	XSI::CValueArray args2(2);
	args2[0] = (XSI::CValue::siPtrType)(desc);
	args2[1] = obj;

	status = app.ExecuteCommand(_W("RComExportMeshData"), args2, retVal);
	if(status == XSI::CStatus::Fail)
		return status;
		
	XSI::CValueArray retArray = retVal;
	//IRVertexBufferGroup* pVBGroupBuffer = (IRVertexBufferGroup*)((ULONG)retArray[0]);
	//IRIndexBuffer* pIBBuffer = (IRIndexBuffer*)((ULONG)retArray[1]);
	
	*ppVBGroup = (IRVertexBufferGroup*)((XSI::CValue::siPtrType)retArray[0]); //LoadVBGroup(g_pRenderer, pVBGroupBuffer, 0);
	*ppIB = (IRIndexBuffer*)((XSI::CValue::siPtrType)retArray[1]); //LoadIB(g_pRenderer, pIBBuffer, 0);
	
	return XSI::CStatus::OK;
}
//==============================================================================
//
//		ExportSceneObject.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/14/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
ExportSceneObject(const wchar* pPath)
{
	XSI::Application app;
	XSI::CRefArray allObjs;
	
	GetAllObjectsWithPolygonMeshes(allObjs, app.GetActiveSceneRoot());
	
	if(allObjs.GetCount() > 0)
	{
		AHSceneObject sceneObj;

		_LOOPi( allObjs.GetCount() )
		{
			XSI::CValue retVal;
			XSI::CValueArray args(1);
			args[0] = allObjs[i];
			XSI::CStatus status = app.ExecuteCommand(_W("BFXComExportBFXMesh"), args, retVal);
			if(status == XSI::CStatus::Fail)
				return status;		
			
			XSI::CValueArray retArray = retVal;
			IBFXMaterialGroup* pMatGroup = (IBFXMaterialGroup*)((XSI::CValue::siPtrType)retArray[0]);
			IRVertexBufferGroup* pVBGroup = (IRVertexBufferGroup*)((XSI::CValue::siPtrType)retArray[1]);
			IRIndexBuffer* pIB = (IRIndexBuffer*)((XSI::CValue::siPtrType)retArray[2]);
			
			BFXMesh mesh;
			mesh.pMatGroup = pMatGroup;
			mesh.pVBGroup = pVBGroup;
			mesh.pIB = pIB;
			
			sceneObj.AddRenderMesh(mesh);						
		}		
		
		// Compute bounds of scene object
		ComputeBounds(sceneObj);
		
		IByteBufferPtr pBuffer = _NEW CByteBuffer(256);		
		SaveSceneObject(&sceneObj, pBuffer);
		
		IFFilePtr pFile = g_pFileSystem->GetFile(pPath);
		pFile->Open(FO_WRITE_ONLY);
		pFile->Write(pBuffer->GetData(), pBuffer->GetDataLength());
		pFile->Close();
		
		return XSI::CStatus::OK;
	}
	
	return XSI::CStatus::Fail;
}
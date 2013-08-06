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
		SceneObject sceneObj;

		_LOOPi( allObjs.GetCount() )
		{
			XSI::CValue retVal;
			XSI::CValueArray args(1);
			args[0] = allObjs[i];
			status = app.ExecuteCommand(_W("RFXComExportRFXMesh"), args, retVal);
			if(status == XSI::CStatus::Fail);
				return status;		
			
			XSI::CValueArray retArray = retVal;
			IRFXEffectGroup* pEffectGroup  = (IRFXEffectGroup*)((ULONG)retArray[0]);
			IRVertexBufferGroup* pVBGroup = (IRVertexBufferGroup*)((ULONG)retArray[0]);
			IRIndexBuffer* pIB = (IRVertexBufferGroup*)((ULONG)retArray[0]);
			
			RFXMesh mesh;
			mesh.pEffectGroup = pEffectGroup;
			mesh.pVBGroup = pVBGroup;
			mesh.pIB = pIB;
			
			sceneObj.AddRenderMesh(mesh);						
		}		
		
		IByteBufferPtr pBuffer = new CByteArray(256);		
		SaveSceneObject(&sceneObj, pBuffer);
		
		IFFilePtr pFile = g_pFileSystem->GetFile(pPath);
		pFile->Open(FO_WRITE_ONLY);
		pFile->Write(pBuffer->GetData(), pBuffer->GetDataLength());
		pFile->Close();
		
		return XSI::CStatus::OK;
	}
	
	return XSI::CStatus::Fail;
}
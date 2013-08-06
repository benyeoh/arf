//==============================================================================
//
//		LoadSaveFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/11/2007
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

//uint
//SaveSceneObject(AHSceneObject* pObj, IByteBuffer* pBuffer)
//{
//	_DEBUG_ASSERT(pObj);
//	_DEBUG_ASSERT(pBuffer);
//
//	uint totalBytesWritten = AppendData(pBuffer, SCENE_OBJECT_CHUNK_H);	
//			
//	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('a','b','o','x'), &pObj->GetLocalAABox());
//	uint numOfMeshes = pObj->GetNumOfRenderMeshes();	
//	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','m','m'), &numOfMeshes);
//							
//	_LOOPi(numOfMeshes)
//	{
//		totalBytesWritten += SaveBFXMesh(&(pObj->GetRenderMesh(i)), pBuffer);		
//	}
//
//	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
//	
//	return totalBytesWritten;
//}
//
//uint
//LoadSceneObject(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, AHSceneObject** ppSceneObj)
//{
//	_DEBUG_ASSERT(pRenderer);
//	_DEBUG_ASSERT(pBaseFX);
//	_DEBUG_ASSERT(pBuffer);
//	
//	uint totalBytesRead = 0;
//	
//	uint header = 0;
//	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);
//
//	if(header == SCENE_OBJECT_CHUNK_H)
//	{
//		*ppSceneObj = _NEW AHSceneObject;
//		
//		totalBytesRead += bytesRead;
//		
//		DataMap store;
//		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
//		
//		uint numMeshes = 0;
//		GetData(&store, _MAKE_ID('n','u','m','m'), &numMeshes);
//		AABox localAABox;
//		GetData(&store, _MAKE_ID('a','b','o','x'), &localAABox);
//		
//		(*ppSceneObj)->SetLocalAABox(localAABox);
//		
//		_LOOPi(numMeshes)
//		{
//			BFXMesh mesh;
//			uint bytesRead = LoadBFXMesh(pRenderer, pBaseFX, pBuffer, offset + totalBytesRead, mesh);
//			if(bytesRead > 0)
//				(*ppSceneObj)->AddRenderMesh(mesh);
//				
//			totalBytesRead += bytesRead;
//		}		
//		
//		byte endHeader = 0;
//		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
//		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
//	}
//	
//	return totalBytesRead;
//}

//uint
//SaveRenderComponent(AHRenderComponent* pRendComp, IByteBuffer* pBuffer)
//{
//	_DEBUG_ASSERT(pRendComp);
//	_DEBUG_ASSERT(pBuffer);
//
//	uint totalBytesWritten = AppendData(pBuffer, COMP_RENDER_CHUNK_H);	
//			
//	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('a','b','o','x'), &pRendComp->GetLocalAABox());
//	const wchar* pFileName = pRendComp->GetRenderMeshFileName();
//	if(pFileName)
//		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('f','i','n','a'), pFileName);
//							
//	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
//	
//	return totalBytesWritten;
//}
//
//uint
//LoadRenderComponent(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, AHRenderComponent** ppRendComp)
//{
//	_DEBUG_ASSERT(pRenderer);
//	_DEBUG_ASSERT(pBaseFX);
//	_DEBUG_ASSERT(pBuffer);
//	
//	uint totalBytesRead = 0;
//	
//	uint header = 0;
//	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);
//
//	if(header == COMP_RENDER_CHUNK_H)
//	{
//		*ppRendComp = _NEW AHRenderComponent;
//		
//		totalBytesRead += bytesRead;
//		
//		DataMap store;
//		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
//		
//		const wchar* pFileName = NULL;
//		GetDataRaw(&store, _MAKE_ID('f','i','n','a'), (void**) &pFileName);
//		AABox localAABox;
//		GetData(&store, _MAKE_ID('a','b','o','x'), &localAABox);
//		
//		(*ppRendComp)->SetLocalAABox(localAABox);
//
//		if(pFileName)
//		{
//			IByteBufferPtr pBFXMeshListBuffer = pBaseFX->GetAppCallback().GetFileData(pFileName);
//			BFXUMeshListPtr pMeshList = _NEW BFXUMeshList;
//			uint bytesRead = LoadBFXUMeshList(pRenderer, pBaseFX, pBFXMeshListBuffer, 0, *pMeshList);
//			if(bytesRead > 0)
//				(*ppRendComp)->SetMeshList(pMeshList);
//		}
//							
//		byte endHeader = 0;
//		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
//		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
//	}
//	
//	return totalBytesRead;
//
//}

_NAMESPACE_END
//==============================================================================
//
//		LoadSaveFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/10/2008
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

uint
SaveMaterialGroupTemplate(IBFXMaterialGroupTemplate* pTemplate, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);
	_DEBUG_ASSERT(pTemplate);
	
	uint totalBytesWritten = 0;
	
	totalBytesWritten += AppendData(pBuffer, BFXU_MAT_GROUP_TEMPLATE_CHUNK_H);
			
	totalBytesWritten += SaveDataKey(pTemplate->GetID(), pBuffer); //SaveData(pBuffer, _W("resource_id"), &pTemplate->GetID());
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('f','i','l','n'), pTemplate->GetFileName());
					
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	
	return totalBytesWritten;
	
}

uint
LoadMaterialGroupTemplate(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, IBFXMaterialGroupTemplate** ppTemplate)
{
	_DEBUG_ASSERT(pBaseFX);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;
	
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);

	if(header == BFXU_MAT_GROUP_TEMPLATE_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		//wchar fileName[512];
		//GetData(&store, _W("filename"), fileName);

		wchar* fileName = NULL;
		GetDataRaw(&store, _MAKE_ID('f','i','l','n'), (void**) &fileName);
		_DEBUG_ASSERT( IsAlignedToXBytes(fileName, strideof(wchar)) );
		
		CRCDataKey id;
		LoadDataKey(store, id);
				
		IBFXMaterialGroupTemplate* pTemplate = pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(&id, NULL, fileName);
		if(!pTemplate)
		{
			IByteBufferPtr pFileBuffer = pBaseFX->GetAppCallback().GetFileData(fileName);
			pTemplate = pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(&id, pFileBuffer, fileName);
		}
		
		_DEBUG_ASSERT(pTemplate);
		*ppTemplate = pTemplate;	

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

uint
SaveMaterialGroup(IBFXMaterialGroup* pMatGroup, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);
	_DEBUG_ASSERT(pMatGroup);
	
	uint totalBytesWritten = 0;
	
	totalBytesWritten += AppendData(pBuffer, BFXU_MAT_GROUP_CHUNK_H);
	
	uint numParams = pMatGroup->GetNumOfParams();//GetCommonParams(pEffectGroup, params, paramSemantics);	
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','m','p'), &numParams);
	totalBytesWritten += SaveDataKey(pMatGroup->GetID(), pBuffer);

	_DEBUG_ASSERT(numParams < 65535);	
	_LOOPi(numParams)
	{
		uint semantic = pMatGroup->GetSemantic(i);
		totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','e',0x00,0x00) | i, &semantic);
	}
	
	_LOOPi(numParams)
	{
		REffectParam param;
		pMatGroup->GetParam(i, param);
		totalBytesWritten += SaveEffectParam(&param, pBuffer);
	}

	totalBytesWritten += SaveMaterialGroupTemplate(pMatGroup->GetTemplate(), pBuffer);
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	
	return totalBytesWritten;
}

uint
LoadMaterialGroup(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, IBFXMaterialGroup** ppGroup)
{
	_DEBUG_ASSERT(pBaseFX);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;
	
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);

	if(header == BFXU_MAT_GROUP_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
			
		std::vector<REffectParam> params;
		std::vector<uint> paramSemantics;
		std::vector<REffectParamStore> paramStores;
		
		uint numParams = 0;
		GetData(&store, _MAKE_ID('n','u','m','p'), &numParams);
		CRCDataKey id;
		LoadDataKey(store, id);
		
		_LOOPi(numParams)
		{
			int semantic = -1;
			if(GetData(&store, _MAKE_ID('s','e',0x00,0x00) | i, &semantic))
				paramSemantics.push_back(semantic);
		}

		_LOOPi(numParams)		
		{
			REffectParamStore paramStore;
			uint bytesRead = LoadEffectParam(pRenderer, pBuffer, offset + totalBytesRead, &paramStore);
			if(bytesRead > 0)
				paramStores.push_back(paramStore);
			
			totalBytesRead += bytesRead;
		}	

		_DEBUG_ASSERT(paramStores.size() == paramSemantics.size());
		
		_LOOPi(paramStores.size())
		{
			REffectParam param;
			paramStores[i].AssignREffectParam(param);

			params.push_back(param);
		}
		
		const REffectParam* pParams = params.size() > 0 ? &(params[0]) : NULL;
		const uint* pParamSemantics = paramSemantics.size() > 0 ? &(paramSemantics[0]) : NULL;

		IBFXMaterialGroupTemplate* pTemplate = NULL;
		totalBytesRead += LoadMaterialGroupTemplate(pRenderer, pBaseFX, pBuffer, offset + totalBytesRead, &pTemplate);
		_DEBUG_ASSERT(pTemplate);		
		
		IBFXMaterialGroup* pMatGroup = 
			pBaseFX->GetResourceMgr().CreateMaterialGroup(&id, pTemplate, pParams, pParamSemantics, numParams);
		
		*ppGroup = pMatGroup;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

uint
SaveBFXMesh(BFXMesh* pMesh, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);
	_DEBUG_ASSERT(pMesh);

	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, BFXU_MESH_CHUNK_H);

	// Save vb group
	totalBytesWritten += SaveVBGroup(pMesh->pVBGroup, pBuffer);
				
	// Save ib
	totalBytesWritten += SaveIB(pMesh->pIB, pBuffer);
	
	// Save mat group
	totalBytesWritten += SaveMaterialGroup(pMesh->pMatGroup, pBuffer);
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	
	return totalBytesWritten;
}

uint
LoadBFXMesh(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, BFXMesh& mesh)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBaseFX);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset, &header);

	if(header == BFXU_MESH_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		// Load VB Group
		IRVertexBufferGroup* pGroup = NULL;
		totalBytesRead += LoadVBGroup(pRenderer, pBuffer, totalBytesRead + offset, &pGroup);
		mesh.pVBGroup = pGroup;
		
		// Load IB
		IRIndexBuffer* pIB = NULL;
		totalBytesRead += LoadIB(pRenderer, pBuffer, totalBytesRead + offset, &pIB);
		mesh.pIB = pIB;
		
		// Load Mat Group
		// offset += ReadData(pBuffer, offset, &chunkSize);
		IBFXMaterialGroup* pMatGroup = NULL;
		totalBytesRead += LoadMaterialGroup(pRenderer, pBaseFX, pBuffer, totalBytesRead + offset, &pMatGroup);
		mesh.pMatGroup = pMatGroup;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;		
}

uint
SaveBFXUMeshList(BFXUMeshList* pMeshList, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);
	_DEBUG_ASSERT(pMeshList);

	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, BFXU_MESH_LIST_CHUNK_H);

	// Save vb group
	uint numMeshes	= (uint) pMeshList->meshes.size();
	totalBytesWritten +=  SaveData(pBuffer, _MAKE_ID('n','u','m','m'), &numMeshes); 
	
	_LOOPi(numMeshes)
	{
		totalBytesWritten += SaveBFXMesh(&(pMeshList->meshes[i]), pBuffer);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	
	return totalBytesWritten;
}

uint
LoadBFXUMeshList(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IByteBuffer* pBuffer, uint offset, BFXUMeshList& meshList)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBaseFX);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset, &header);

	if(header == BFXU_MESH_LIST_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		uint numMeshes = 0;
		GetData(&store, _MAKE_ID('n','u','m','m'), &numMeshes);
		_LOOPi(numMeshes)
		{
			BFXMesh mesh;
			totalBytesRead += LoadBFXMesh(pRenderer, pBaseFX, pBuffer, offset + totalBytesRead, mesh);
			
			meshList.meshes.push_back(mesh);		
		}
				
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;		
}


_NAMESPACE_END
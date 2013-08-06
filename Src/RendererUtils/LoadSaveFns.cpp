//==============================================================================
//
//		LoadSaveFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2007
//
//==============================================================================

#include "RendererUtils.h"

_NAMESPACE_BEGIN

uint
SaveVBGroup(IRVertexBufferGroup* pVBGroup, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pVBGroup);
	_DEBUG_ASSERT(pBuffer);
	
	uint numVB = pVBGroup->GetNumOfVertexBuffers();
	uint totalBytesWritten = 0;
	
	totalBytesWritten += AppendData(pBuffer, RU_VBGROUP_CHUNK_H);
	totalBytesWritten += SaveDataKey(pVBGroup->GetResourceID(), pBuffer);

	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','m','v','b'), &numVB);
	
	_LOOPi(numVB)
	{
		IRVertexBuffer* pVB = pVBGroup->GetVertexBuffer(i);
		_DEBUG_ASSERT(pVB);
		
		totalBytesWritten += SaveVB(pVB, pBuffer);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);

	return totalBytesWritten;
}

uint 
SaveVB(IRVertexBuffer* pVB, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pVB);
	_DEBUG_ASSERT(pBuffer);
	
	uint totalBytesWritten = 0;
	
	totalBytesWritten += AppendData(pBuffer, RU_VB_CHUNK_H);
	totalBytesWritten += SaveDataKey(pVB->GetResourceID(), pBuffer);
	
	const RVertexDesc* pDesc = pVB->GetDescription();
	uint sizeDesc = sizeof(RVertexDesc);
	uint index = 0;
	while(true)
	{
		if(pDesc[index].usage == VDU_END)
			break;
		
		sizeDesc += sizeof(RVertexDesc);
		index++;
	}
	
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('d','e','s','c'), pDesc, sizeDesc, strideof(RVertexDesc));
	uint numVerts = pVB->GetNumVertices();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','m','v'), &numVerts);
	uint startVert = pVB->GetStartVertex();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','t','a','v'), &startVert);
	uint usage = (uint) pVB->GetUsage();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('u','s','a','g'), &usage);
	uint size = pVB->GetSize();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','i','z','e'), &size);
	byte* pData = pVB->Lock(0, 0); // Read-only	
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('d','a','t','a'), pData, size * pVB->GetVertexSize());
	pVB->Unlock(FALSE);

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	return totalBytesWritten;
}

uint
LoadVB(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRVertexBuffer** ppVB)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBuffer);
	
	uint totalBytesRead = 0;
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset, &header);
	if(header == RU_VB_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		CRCDataKey resID;
		LoadDataKey(store, resID);

		RVertexDesc* pDesc = NULL;
		GetDataRaw(&store, _MAKE_ID('d','e','s','c'), (void**) &pDesc);
		_DEBUG_ASSERT( IsAlignedToXBytes(pDesc, strideof(RVertexDesc)) );
		
		uint numVerts = 0;
		GetData(&store, _MAKE_ID('n','u','m','v'), &numVerts);
		uint startVertex = 0;
		GetData(&store, _MAKE_ID('s','t','a','v'), &startVertex);
		uint usage = 0;
		GetData(&store, _MAKE_ID('u','s','a','g'), &usage);
		
		// Total number of vertices (also the ones not in use)
		uint size = 0;
		GetData(&store, _MAKE_ID('s','i','z','e'), &size);
		
		// Check if it's already been created
		IRVertexBuffer* pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(&resID, NULL, 0, (eRBufferUsage) usage);
		if(!pVB)
		{
			pVB = pRenderer->GetRResourceMgr().CreateVertexBuffer(&resID, pDesc, size, (eRBufferUsage) usage);	
			
			byte* pDest = pVB->Lock(0, size);	// Lock the whole buffer
			GetData(&store, _MAKE_ID('d','a','t','a'), pDest);
			boolean result = pVB->Unlock();
			_DEBUG_ASSERT(result);
			
			pVB->SetValidRange(startVertex, numVerts);
		}
		
		_DEBUG_ASSERT(pVB);
		
		*ppVB = pVB;
	
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

uint
LoadVBGroup(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRVertexBufferGroup** ppGroup)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBuffer);
	
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == RU_VBGROUP_CHUNK_H)
	{
		totalBytesRead += numBytes;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		CRCDataKey resID;
		LoadDataKey(store, resID);

		uint numVBs = 0;
		GetData(&store, _MAKE_ID('n','m','v','b'), &numVBs);
		
		IRVertexBufferGroup* pVBGroup = pRenderer->GetRResourceMgr().CreateVertexBufferGroup(&resID);
		boolean isBeenLoaded = pVBGroup->GetNumOfVertexBuffers() > 0;
		_LOOPi(numVBs)
		{
			IRVertexBuffer* pVB = NULL;
			totalBytesRead += LoadVB(pRenderer, pBuffer, offset + totalBytesRead, &pVB);
			
			if(!isBeenLoaded && pVB)
				pVBGroup->AddVertexBuffer(pVB);
		}	
				
		*ppGroup = pVBGroup;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

uint 
SaveIB(IRIndexBuffer* pIB, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pIB);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, RU_IB_CHUNK_H);
	totalBytesWritten += SaveDataKey(pIB->GetResourceID(), pBuffer);

	uint numIndices = pIB->GetNumIndices();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','m','i'), &numIndices);
	uint startIndex = pIB->GetStartIndex();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','t','a','i'), &startIndex);
	uint usage = (uint) pIB->GetUsage();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('u','s','a','g'), &usage);
	uint size = pIB->GetSize();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','i','z','e'), &size);

	ushort* pData = pIB->Lock(0, 0); // Read-only	
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('d','a','t','a'), pData, size * sizeof(ushort));
	pIB->Unlock(FALSE);

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	
	return totalBytesWritten;
}

uint
LoadIB(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRIndexBuffer** ppIB)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBuffer);
	
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);
	if(header == RU_IB_CHUNK_H)
	{
		totalBytesRead += numBytes;
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		CRCDataKey resID;
		LoadDataKey(store, resID);

		uint numIndices = 0;
		GetData(&store, _MAKE_ID('n','u','m','i'), &numIndices);
		uint startIndex = 0;
		GetData(&store, _MAKE_ID('s','t','a','i'), &startIndex);
		uint usage = 0;
		GetData(&store, _MAKE_ID('u','s','a','g'), &usage);
		uint size = 0;
		GetData(&store, _MAKE_ID('s','i','z','e'), &size);

		// Check if it's already been created
		IRIndexBuffer* pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(&resID, 0, (eRBufferUsage) usage);
		if(!pIB)
		{
			_DEBUG_ASSERT(size > 0);
			pIB = pRenderer->GetRResourceMgr().CreateIndexBuffer(&resID, size, (eRBufferUsage) usage);
			ushort* pDest = pIB->Lock(0, size);
			GetData(&store, _MAKE_ID('d','a','t','a'), pDest);
			boolean result = pIB->Unlock();
			_DEBUG_ASSERT(result);
			
			pIB->SetValidRange(startIndex, numIndices);

		}

		_DEBUG_ASSERT(pIB);

		*ppIB = pIB;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
SaveEffectParam(const REffectParam* pParam, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pParam);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = 0;
	
	totalBytesWritten += AppendData(pBuffer, RU_EFFECT_PARAM_CHUNK_H);

	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('t','y','p','e'), (int*) &(pParam->type));
		
	switch(pParam->type)
	{
		case EPT_TEX2D_P:
		{
			totalBytesWritten += SaveDataKey(pParam->pTex->GetResourceID(), pBuffer);			
			totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pTex->GetFileName());
			//totalBytesWritten += AppendData(pBuffer, pParam->pTex->GetResourceID());			
			//totalBytesWritten += AppendData(pBuffer, pParam->pTex->GetFileName());
			break;
		}
		
		case EPT_TEXCUBE_P:
		{
			totalBytesWritten += SaveDataKey(pParam->pCube->GetResourceID(), pBuffer);			
			totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pCube->GetFileName());
			break;
		}	

		case EPT_VOID_P:
		{
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','m','b'), &pParam->numBytes);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pVal, pParam->numBytes);
			break;
		}
		
		case EPT_MAT44_P:
		{
			uint numBytes = sizeof(gmtl::Matrix44f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pMat44, numBytes, strideof(gmtl::Matrix44f));
			break;
		}
		
		case EPT_MAT33_P:
		{
			uint numBytes = sizeof(gmtl::Matrix33f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pMat33, numBytes, strideof(gmtl::Matrix33f));
			break;
		}
		
		case EPT_MAT22_P:
		{
			uint numBytes = sizeof(gmtl::Matrix22f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pMat22, numBytes, strideof(gmtl::Matrix22f));
			break;
		}

		case EPT_VEC4_P:
		{
			uint numBytes = sizeof(gmtl::Vec4f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pVec4, numBytes, strideof(gmtl::Vec4f));
			break;
		}

		case EPT_VEC3_P:
		{
			uint numBytes = sizeof(gmtl::Vec3f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pVec3, numBytes, strideof(gmtl::Vec3f));
			break;
		}

		case EPT_VEC2_P:
		{
			uint numBytes = sizeof(gmtl::Vec2f);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pVec2, numBytes,  strideof(gmtl::Vec2f));
			break;
		}

		case EPT_FLOAT:
		{
			//totalBytesWritten += AppendData(pBuffer, numBytes);
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), &pParam->floatVal);
			break;
		}

		case EPT_INT:
		{
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), &pParam->intVal);
			break;
		}

		case EPT_BOOL:
		{
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','a','l','u'), &pParam->boolVal);
			break;
		}

		case EPT_STRING:
		{
			totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l','u'), pParam->pStr);
			break;
		}

		default: _DEBUG_ASSERT(FALSE); break;
	}

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	return totalBytesWritten;
}

uint 
LoadEffectParam(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, REffectParamStore* pParam)
{	
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;
	
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);
	if(header == RU_EFFECT_PARAM_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		int paramType = EPT_END;
		GetData(&store, _MAKE_ID('t','y','p','e'), &paramType);
		pParam->type = (eREffectParamType) paramType;
			
		switch(pParam->type)
		{
		case EPT_TEX2D_P:
			{	
				CRCDataKey resID;
				LoadDataKey(store, resID);
				
				const wchar* pFileName = NULL;
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pFileName);
				_DEBUG_ASSERT( IsAlignedToXBytes((void*) pFileName, strideof(wchar)) );

				IRTexture2D* pTex = pRenderer->GetRResourceMgr().LoadTexture2D(&resID, NULL);
				if(!pTex)
				{
					IByteBufferPtr pFileBuffer = pRenderer->GetAppCallback().GetFileData(pFileName);							
					pTex = pRenderer->GetRResourceMgr().LoadTexture2D(&resID, pFileBuffer);
				}
				
				pTex->SetFileName(pFileName);			
				pParam->SetTexture2D(pTex);				
				break;
			}
		case EPT_TEXCUBE_P:
			{
				CRCDataKey resID;
				LoadDataKey(store, resID);
			
				const wchar* pFileName = NULL;
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pFileName);
				_DEBUG_ASSERT( IsAlignedToXBytes((void*) pFileName, strideof(wchar)) );

				IRTextureCube* pCube = pRenderer->GetRResourceMgr().LoadTextureCube(&resID, NULL);
				if(!pCube)
				{
					IByteBufferPtr pFileBuffer = pRenderer->GetAppCallback().GetFileData(pFileName);							
					pCube = pRenderer->GetRResourceMgr().LoadTextureCube(&resID, pFileBuffer);
				}

				pCube->SetFileName(pFileName);			
				pParam->SetTextureCube(pCube);				
				break;
			}
			
		case EPT_VOID_P:
			{
				uint numBytes = 0;
				GetData(&store, _MAKE_ID('n','u','m','b'), &numBytes);
				void* pData = NULL;
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &pData);
				pParam->SetValue(pData, numBytes);
				break;
			}

		case EPT_MAT44_P:
			{
				void* val = NULL;
				//float val[sizeof(gmtl::Matrix44f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Matrix44f)) );
				pParam->SetMatrix44((gmtl::Matrix44f*) val);				
				break;
			}

		case EPT_MAT33_P:
			{
				void* val = NULL;//[sizeof(gmtl::Matrix33f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Matrix33f)) );
				pParam->SetMatrix33((gmtl::Matrix33f*) val);				
				break;
			}

		case EPT_MAT22_P:
			{
				void* val = NULL;//[sizeof(gmtl::Matrix33f) / sizeof(float)];
				//float val[sizeof(gmtl::Matrix22f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Matrix22f)) );
				pParam->SetMatrix22((gmtl::Matrix22f*) val);				
				break;
			}

		case EPT_VEC4_P:
			{
				void* val = NULL;//[sizeof(gmtl::Matrix33f) / sizeof(float)];
				//float val[sizeof(gmtl::Vec4f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Vec4f)) );
				pParam->SetVector4((gmtl::Vec4f*) val);				
				break;
			}

		case EPT_VEC3_P:
			{
				void* val = NULL;//[sizeof(gmtl::Matrix33f) / sizeof(float)];
				//float val[sizeof(gmtl::Vec3f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Vec3f)) );
				pParam->SetVector3((gmtl::Vec3f*) val);				
				break;
			}

		case EPT_VEC2_P:
			{
				void* val = NULL;//[sizeof(gmtl::Matrix33f) / sizeof(float)];
				//float val[sizeof(gmtl::Vec2f) / sizeof(float)];
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), &val);
				_DEBUG_ASSERT( IsAlignedToXBytes(val, strideof(gmtl::Vec2f)) );
				pParam->SetVector2((gmtl::Vec2f*) val);				
				break;
			}

		case EPT_FLOAT:
			{
				float val = 0.0f;
				GetData(&store, _MAKE_ID('v','a','l','u'), &val);
				pParam->SetFloat(val);
				break;
			}

		case EPT_INT:
			{
				int val = 0;
				GetData(&store, _MAKE_ID('v','a','l','u'), &val);
				pParam->SetInt(val);
				break;
			}

		case EPT_BOOL:
			{
				boolean val = FALSE;
				GetData(&store, _MAKE_ID('v','a','l','u'), &val);
				pParam->SetBoolean(val);
				break;
			}

		case EPT_STRING:
			{
				//wchar val[512];
				//GetData(&store, _W("value"), val);
				const wchar* pStr = NULL;
				GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pStr);				
				pParam->SetString(pStr);
				break;
			}

		default: _DEBUG_ASSERT(FALSE); break;
		}
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}


uint
SaveEffect(IREffect* pEffect, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pEffect);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, RU_EFFECT_CHUNK_H);
	
	totalBytesWritten += SaveDataKey(pEffect->GetTemplate()->GetResourceID(), pBuffer);
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','f','l','n'), pEffect->GetTemplateFileName());
	uint numParams = pEffect->GetNumOfParams();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','u','p','a'), &numParams);

	_DEBUG_ASSERT(numParams < 65535);
	_LOOPi(numParams)
	{
		uint tag = _MAKE_ID('s','m', 0x00, 0x00);
		tag |= i;
		
		uint semantic = pEffect->GetTemplate()->GetConstantParamSemantic(i);
		totalBytesWritten += SaveData(pBuffer, tag, &semantic);
	}
	
	_LOOPi(numParams)
	{
		const REffectParam* pParam = pEffect->GetParam(i);
		_DEBUG_ASSERT(pParam);
		
		totalBytesWritten += SaveEffectParam(pParam, pBuffer);
	}

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);
	return totalBytesWritten;
}

uint
LoadEffect(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IREffect** ppEffect)
{
	_DEBUG_ASSERT(pRenderer);
	_DEBUG_ASSERT(pBuffer);

	IREffect* pEffect =  NULL;
	
	uint totalBytesRead = 0;
	
	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);
	
	if(header == RU_EFFECT_CHUNK_H)
	{
		totalBytesRead += bytesRead;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		CRCDataKey resID;
		LoadDataKey(store, resID);
		
		wchar* fileName = NULL;
		GetDataRaw(&store, _MAKE_ID('t','f','l','n'), (void**) &fileName);
		_DEBUG_ASSERT( IsAlignedToXBytes(fileName, strideof(wchar)) );
		
		uint numParams = 0;
		GetData(&store, _MAKE_ID('n','u','p','a'), &numParams);		
		
		std::vector<REffectParamStore> paramStores;
		std::vector<REffectParam> params;
		std::vector<uint> paramSemantics;
		
		_LOOPi(numParams)
		{
			uint tag = _MAKE_ID('s','m', 0x00, 0x00);
			tag |= i;
		
			int semantic;
			if(GetData(&store, tag, &semantic))
				paramSemantics.push_back((uint)semantic);
		}
		
		_LOOPi(numParams)
		{		
			REffectParamStore store;
			store.type = EPT_END;
			paramStores.push_back( store );
			REffectParamStore& paramStore = paramStores.back();
			totalBytesRead += LoadEffectParam(pRenderer, pBuffer, offset + totalBytesRead, &paramStore);
			if(paramStore.type != EPT_END)
			{
				//paramStores.push_back(paramStore);
				REffectParam param;
				paramStore.AssignREffectParam(param);
				params.push_back(param);
			}
			else
			{
				paramStores.pop_back();
			}
		}
			
		const REffectParam* pParams = params.size() > 0 ? &(params[0]) : NULL;
		const uint* pParamSemantics = paramSemantics.size() > 0 ? &(paramSemantics[0]) : NULL;
		
		_DEBUG_ASSERT(params.size() == paramSemantics.size());
		_DEBUG_ASSERT((uint)(params.size()) == numParams);
		
		IREffect* pEffect = pRenderer->GetRResourceMgr().LoadEffect(&resID, NULL, pParams, pParamSemantics, numParams);
		if(!pEffect)
		{
			IByteBufferPtr pFileDataBuffer = pRenderer->GetAppCallback().GetFileData(fileName);
			pEffect = pRenderer->GetRResourceMgr().LoadEffect(&resID, pFileDataBuffer, pParams, pParamSemantics, numParams);
		}
		
		pEffect->SetTemplateFileName(fileName);
		
		*ppEffect = pEffect;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

_NAMESPACE_END
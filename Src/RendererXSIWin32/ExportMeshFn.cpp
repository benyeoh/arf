//==============================================================================
//
//		ExportMeshFn.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/9/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
ExtractPositions(XSI::X3DObject& obj, const RVertexDesc& desc, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(desc.usage == VDU_POSITION);
	_DEBUG_ASSERT(pBuffer);

	XSI::PolygonMesh polyMesh = obj.GetActivePrimitive().GetGeometry();
	XSI::CGeometryAccessor geomAcc = polyMesh.GetGeometryAccessor(XSI::siConstructionModeModeling);

	if(desc.usage != VDU_POSITION)
		return XSI::CStatus::Fail;
	
	XSI::MATH::CMatrix4 xsiTrans = obj.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
	gmtl::Matrix44f mat;
	XSIToGMTL(mat, xsiTrans);
		
	XSI::CLongArray vertIndices;
	geomAcc.GetTriangleVertexIndices(vertIndices);
	XSI::CDoubleArray vertices;
	geomAcc.GetVertexPositions(vertices);
	XSI::CLongArray nodeIndices;
	geomAcc.GetTriangleNodeIndices(nodeIndices);

	LONG numNodes = geomAcc.GetNodeCount();

	// Reset buffer
	pBuffer->SetDataLength(0);
	pBuffer->ResizeBuffer((uint)(numNodes * sizeof(float) * 3));

	int numIndices = (int) nodeIndices.GetCount();	
	
	_LOOPi(numIndices)
	{			
		LONG nodeIndex = nodeIndices[i];
		LONG vertIndex = vertIndices[i];
		
		gmtl::Vec4f pos;
		pos[0] = (float) vertices[vertIndex*3];
		pos[1] = (float) vertices[vertIndex*3+1];
		pos[2] = (float) vertices[vertIndex*3+2];
		pos[3] = 1.0f;
		
		gmtl::Vec4f transPos;

		// Transform vertices
		gmtl::xform(transPos, mat, pos);
		
		byte* pData = pBuffer->GetData() + nodeIndex * sizeof(float) * 3;
		memcpy(pData, transPos.getData(), sizeof(float) * 3);
	}
	
	pBuffer->SetDataLength((uint)(numNodes * sizeof(float) * 3));

	return XSI::CStatus::OK;
}

XSI::CStatus
ExtractNormals(XSI::X3DObject& obj, const RVertexDesc& desc, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(desc.usage == VDU_NORMAL);
	_DEBUG_ASSERT(pBuffer);
	
	XSI::PolygonMesh polyMesh = obj.GetActivePrimitive().GetGeometry();
	XSI::CGeometryAccessor geomAcc = polyMesh.GetGeometryAccessor(XSI::siConstructionModeModeling);

	if(desc.usage != VDU_NORMAL)
		return XSI::CStatus::Fail;

	XSI::MATH::CMatrix4 xsiTrans = obj.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
	gmtl::Matrix44f mat;
	XSIToGMTL(mat, xsiTrans);

	//int numNodes = (int) geomAcc.GetNodeCount();
	//CFloatArray norms;
	//geomAcc.GetNodeNormals(norms);
	//
	//_LOOPi(numNodes)
	//{
	//	gmtl::Vec3f norm;
	//	norm[0] = (float) norms[i*3];
	//	norm[1] = (float) norms[i*3+1];
	//	norm[2] = (float) norms[i*3+2];
	//	
	//	byte* pData = pBuffer->GetData() + i * sizeof(float) * 3;
	//	memcpy(pData, norm.getData(), sizeof(float) * 3);
	//}
	//
	//pBuffer->SetDataLength((uint)(numNodes * sizeof(float) * 3));

	LONG numNodes = geomAcc.GetNodeCount();

	// Reset buffer
	pBuffer->SetDataLength(0);
	pBuffer->ResizeBuffer((uint)(numNodes * sizeof(float) * 3));
	
	XSI::CLongArray nodeIndices;
	geomAcc.GetTriangleNodeIndices(nodeIndices);

	XSI::CTriangleRefArray triArray = polyMesh.GetTriangles();
	int numTris = (int) triArray.GetCount();
	_LOOPi(numTris)
	{
		XSI::Triangle tri(triArray[i]);
		XSI::CTriangleVertexRefArray points = tri.GetPoints();
		_LOOPj(3)
		{
			XSI::TriangleVertex point(points[j]);
			XSI::MATH::CVector3 norm = point.GetNormal();
			gmtl::Vec3f theNormal;
			theNormal[0] = (float) norm.GetX();
			theNormal[1] = (float) norm.GetY();
			theNormal[2] = (float) norm.GetZ();
			//theNormal[3] = 0.0f;

			// Transform vertices
			gmtl::VecA3f transNorm;
			gmtl::xform(_CAST_VEC3(transNorm), mat, theNormal);
			NormalizeVec(transNorm);
			
			LONG nodeIndex = nodeIndices[i*3+j];
			
			byte* pData = pBuffer->GetData() + nodeIndex * sizeof(float) * 3;
			memcpy(pData, transNorm.getData(), sizeof(float) * 3);
		}
	}
	
	pBuffer->SetDataLength((uint)(numNodes * sizeof(float) * 3));

	return XSI::CStatus::OK;
}

XSI::CStatus
ExtractUVs(XSI::X3DObject& obj, const RVertexDesc& desc, IByteBuffer* pBuffer)
{
	XSI::PolygonMesh polyMesh = obj.GetActivePrimitive().GetGeometry();
	XSI::CGeometryAccessor geomAcc = polyMesh.GetGeometryAccessor(XSI::siConstructionModeModeling);

	LONG numNodes = geomAcc.GetNodeCount();
	
	// Reset buffer
	pBuffer->SetDataLength(0);
	pBuffer->ResizeBuffer((uint)(numNodes * sizeof(float) * 3));

	XSI::CLongArray nodeIndices;
	geomAcc.GetTriangleNodeIndices(nodeIndices);
	int numIndices = (int) nodeIndices.GetCount();
	
	XSI::CRefArray uvChannels = geomAcc.GetUVs();
	uint numChannels = (uint) uvChannels.GetCount();
	
	uint uvChannelToUse = desc.usageIndex;
	
	if(uvChannelToUse < numChannels)
	{
		XSI::ClusterProperty uvProp = uvChannels[uvChannelToUse];
		XSI::CFloatArray uvValues;
		uvProp.GetValues(uvValues);
		
		_LOOPi(numIndices)
		{
			int uvIndex = nodeIndices[i] * 3;
			float u = uvValues[uvIndex];
			float v = uvValues[uvIndex+1];
			float w = uvValues[uvIndex+2];

			if(desc.usage == VDU_TEXCOORDF2)
			{
				gmtl::Vec2f uv;
				uv[0] = u;
				uv[1] = 1.0f - v;
				
				byte* pData = pBuffer->GetData() + nodeIndices[i] * sizeof(float) * 2;
				memcpy(pData, uv.getData(), sizeof(float) * 2);
			}
			//else
			//if(desc.type == VDT_FLOAT3)
			//{
			//	gmtl::Vec3f uvt;
			//	uvt[0] = u;
			//	uvt[1] = 1.0f - v;
			//	uvt[2] = w;
			//	
			//	byte* pData = pBuffer->GetData() + nodeIndices[i] * sizeof(float) * 3;
			//	memcpy(pData, uvt.getData(), sizeof(float) * 3);
			//}			
			else
			{
				_DEBUG_ASSERT(FALSE);
				return XSI::CStatus::Fail;
			}
		}
		
		if(desc.usage == VDU_TEXCOORDF2)
		{
			pBuffer->SetDataLength((uint)(numNodes * sizeof(float) * 2));
		}
		//else
		//if(desc.type == VDT_FLOAT3)
		//{
		//	pBuffer->SetDataLength((uint)(numNodes * sizeof(float) * 3));
		//}
		else
		{
			_DEBUG_ASSERT(FALSE);
			return XSI::CStatus::Fail;
		}
		
		return XSI::CStatus::OK;
	}
	
	return XSI::CStatus::Fail;
}

XSI::CStatus
ExtractBlendIndices(XSI::X3DObject& obj, const RVertexDesc& desc, IByteBuffer* pBuffer)
{
	return XSI::CStatus::Fail;
}

XSI::CStatus
ExtractBlendWeights(XSI::X3DObject& obj, const RVertexDesc& desc, IByteBuffer* pBuffer)
{
	return XSI::CStatus::Fail;
}

IRIndexBuffer*
ExtractIndices(XSI::X3DObject& obj)
{
	XSI::PolygonMesh polyMesh = obj.GetActivePrimitive().GetGeometry();
	XSI::CGeometryAccessor geomAcc = polyMesh.GetGeometryAccessor(XSI::siConstructionModeModeling);

	XSI::CLongArray nodeIndices;
	geomAcc.GetTriangleNodeIndices(nodeIndices);
	int numIndices = (int) nodeIndices.GetCount();
	
	IByteBufferPtr pIndexData = _NEW CByteBuffer((uint)(numIndices * sizeof(ushort)));
	ushort* pData = (ushort*) pIndexData->GetData();
	
	int numTris = numIndices / 3;
	
	// Flip winding order
	_LOOPi(numTris)
	{
		int temp = nodeIndices[i*3+2];
		nodeIndices[i*3+2] = nodeIndices[i*3+1];
		nodeIndices[i*3+1] = temp;		
	}	
	
	_LOOPi(numIndices)
	{
		*(pData + i)= (ushort) (nodeIndices[i]);		
	}
	
	pIndexData->SetDataLength((uint)(numIndices * sizeof(ushort)));
	
	CRCDataKey id;
	id.Set(pIndexData->GetData(), pIndexData->GetDataLength());
	IRIndexBuffer* pIB = g_pRenderer->GetRResourceMgr().CreateIndexBuffer(&id, numIndices, BUF_DEFAULT);
	ushort* pDest = pIB->Lock(0, numIndices);
	memcpy(pDest, pIndexData->GetData(), pIndexData->GetDataLength());	
	pIB->Unlock();
	return pIB;
}

uint
WeldSimilarVertices(IByteBuffer* pVBData, uint vertexSize, IRIndexBuffer* pIB)
{
	_DEBUG_ASSERT(vertexSize < 1024);
	
	byte zeroData[1024];
	memset(zeroData, 0, 1024);
		
	//DenseChainedHashMap<uint, uint, NullUintHash> vertexRemap;
	//vertexRemap.Initialize(1024, 0.667f);
	
	byte* pData = pVBData->GetData();
	uint numVertices = pVBData->GetDataLength() / vertexSize;
	
	// Fill vertex index
	uint* origToCurrent = _NEW uint[numVertices];
	uint* currentToOrig	= _NEW uint[numVertices];
	boolean* currentIsRemapped = _NEW boolean[numVertices];
	
	_LOOPi(numVertices)
	{
		origToCurrent[i] = i;
		currentToOrig[i] = i;
		currentIsRemapped[i] = FALSE;
	}
		
	_LOOPi(numVertices)
	{
		// If it already has been retargeted
		if(currentIsRemapped[i])
			continue;

		byte* pCurData = pData + i * vertexSize;		

		// Check to see which other vertex matches this
		_LOOPj(numVertices-i-1)
		{
			uint index = j+i+1;
			byte* pToCompare = pData + index * vertexSize;
			if(memcmp(pCurData, pToCompare, vertexSize) == 0)
			{
				currentIsRemapped[index] = TRUE;
				origToCurrent[index] = i;
			}
		}
	}
	
	_LOOPi(numVertices)
	{
		// If it has been retargeted
		if(currentIsRemapped[i])
		{
			_DEBUG_ASSERT(origToCurrent[currentToOrig[i]] < (uint)i);
			
			_LOOPj(numVertices-i-1)
			{
				uint destIndex = j+i;
				uint srcIndex = j+i+1;
		
				byte* pDest = pData + destIndex * vertexSize;
				byte* pSrc = pData + srcIndex * vertexSize;
				memcpy(pDest, pSrc, vertexSize);
				
				currentIsRemapped[destIndex] = currentIsRemapped[srcIndex];
				currentToOrig[destIndex] = currentToOrig[srcIndex];
				
				if(origToCurrent[currentToOrig[destIndex]] > (uint)i)
					origToCurrent[currentToOrig[destIndex]] -= 1;
					
				_DEBUG_ASSERT(!currentIsRemapped[ origToCurrent[currentToOrig[destIndex]] ]);
			}
			
			numVertices--;
			i--;
		}		
	}
	
	pVBData->SetDataLength(numVertices * vertexSize);
	
	// Remap indices
	// Lock for read
	ushort* pReadTemp = pIB->Lock(0, 0);
	ushort* pReadIndices = _NEW ushort[pIB->GetNumIndices()];
	memcpy(pReadIndices, pReadTemp, sizeof(ushort) * pIB->GetNumIndices());
	pIB->Unlock(FALSE);

	// Lock for write now
	ushort* pIndices = pIB->Lock(0, pIB->GetNumIndices());
	_LOOPi(pIB->GetNumIndices())
	{
		pIndices[i] = (ushort) origToCurrent[pReadIndices[i]];
	}
	
	pIB->Unlock(TRUE);

	_DELETE_ARRAY(pReadIndices);
	_DELETE_ARRAY(origToCurrent);
	_DELETE_ARRAY(currentToOrig);
	_DELETE_ARRAY(currentIsRemapped);
	
	return numVertices;
}

IRVertexBuffer* 
ExtractVertices(XSI::X3DObject& obj, const RVertexDesc* pDesc, RVertexDesc* pModDesc, IRIndexBuffer* pIB)
{
	_DEBUG_ASSERT(pDesc);
	_DEBUG_ASSERT(pIB);
	
	XSI::PolygonMesh polyMesh = obj.GetActivePrimitive().GetGeometry();
	XSI::CGeometryAccessor geomAcc = polyMesh.GetGeometryAccessor(XSI::siConstructionModeModeling);

	uint idx = 0;
	std::vector<IByteBufferPtr> verticesData;
	boolean generateTangents = FALSE;
	boolean generateBinormals = FALSE;

	XSI::Application app;

	uint newIdx = 0;
	while(pDesc[idx].usage != VDU_END)
	{
		switch(pDesc[idx].usage)
		{
			case VDU_POSITION:		
			{
				verticesData.push_back( _NEW CByteBuffer(256) );		
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				XSI::CStatus status = ExtractPositions(obj, pDesc[idx], verticesData.back());
				if(status.Succeeded())
				{
					pModDesc[newIdx] = pDesc[idx];
					newIdx++;
				}
				else
				{
					verticesData.pop_back();
				}

				status.AssertSucceeded(_W("Failed to extract position"));
				break;
			}
			
			case VDU_NORMAL:		
			{
				verticesData.push_back( _NEW CByteBuffer(256) );
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				XSI::CStatus status = ExtractNormals(obj, pDesc[idx], verticesData.back());
				if(status.Succeeded())
				{
					pModDesc[newIdx] = pDesc[idx];
					newIdx++;
				}
				else
				{
					verticesData.pop_back();
				}

				status.AssertSucceeded(_W("Failed to extract normals"));
				break;
			}
			
			case VDU_TEXCOORDF2:		
			{
				verticesData.push_back( _NEW CByteBuffer(256) );		
				XSI::CStatus status = ExtractUVs(obj, pDesc[idx], verticesData.back());
				if(status.Succeeded())
				{
					pModDesc[newIdx] = pDesc[idx];
					newIdx++;
				}
				else
				{
					verticesData.pop_back();
				}

				status.AssertSucceeded(_W("Failed to extract uv"));
				break;
			}
			
			case VDU_BLENDINDICES:	
			{
				verticesData.push_back( _NEW CByteBuffer(256) );
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				XSI::CStatus status = ExtractBlendIndices(obj, pDesc[idx], verticesData.back());	
				if(status.Succeeded())
				{
					pModDesc[newIdx] = pDesc[idx];
					newIdx++;
				}
				else
				{
					verticesData.pop_back();
				}

				status.AssertSucceeded(_W("Failed to extract blend indices"));
				break;
			}
			
			case VDU_BLENDWEIGHT:	
			{
				verticesData.push_back( _NEW CByteBuffer(256) );		
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				XSI::CStatus status = ExtractBlendWeights(obj, pDesc[idx], verticesData.back());	
				if(status.Succeeded())
				{
					pModDesc[newIdx] = pDesc[idx];
					newIdx++;
				}
				else
				{
					verticesData.pop_back();
				}

				status.AssertSucceeded(_W("Failed to extract blend weights"));
				break;
			}
			
			case VDU_TANGENT:		
			{
				verticesData.push_back( NULL );		
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				uint texUVIndex = 0;
				while(texUVIndex < newIdx && pModDesc[texUVIndex].usage != VDU_END)
				{
					if(pModDesc[texUVIndex].usage == VDU_TEXCOORDF2)
					{	
						if(pModDesc[texUVIndex].usageIndex == 0)
						{
							pModDesc[newIdx] = pDesc[idx];
							newIdx++;
							generateTangents = TRUE;
							break;
						}
					}

					texUVIndex++;
				}
				break;
			}
			
			case VDU_BINORMAL:		
			{
				verticesData.push_back( NULL );		
				_DEBUG_ASSERT(pDesc[idx].usageIndex == 0);
				
				uint texUVIndex = 0;
				while(texUVIndex < newIdx && pModDesc[texUVIndex].usage != VDU_END)
				{
					if(pModDesc[texUVIndex].usage == VDU_TEXCOORDF2)
					{	
						if(pModDesc[texUVIndex].usageIndex == 0)
						{
							pModDesc[newIdx] = pDesc[idx];
							newIdx++;
							generateBinormals = TRUE;
							break;
						}
					}

					texUVIndex++;
				}
				break;	
			}
			
			// Add VDU_COLOR
			
			default:	_DEBUG_ASSERT(FALSE);	break;	
		}

		idx++;		
	}

	pModDesc[newIdx].usage = VDU_END;
	pModDesc[newIdx].usageIndex = 0;

	uint vertexSize = GetVertexSize(pModDesc);
	
	uint numNodes = (uint) geomAcc.GetNodeCount();
	
	idx = 0;
	IByteBufferPtr pVertexData = _NEW CByteBuffer(numNodes * vertexSize);
	byte* pToFill = pVertexData->GetData();
	
	while(pModDesc[idx].usage != VDU_END)
	{
		//if(pModDesc[idx].usage != VDU_TANGENT &&
		//	pModDesc[idx].usage != VDU_BINORMAL)
		{
			uint dataSize = GetVertexDataSize(pModDesc[idx].usage);
			uint vertexOffset = GetVertexOffset(pModDesc, pModDesc[idx].usage, pModDesc[idx].usageIndex);
			IByteBuffer* pBBData = verticesData[idx];
	
			if(pBBData)				
			{
				_LOOPi(numNodes)
				{
					const byte* pData = pBBData->GetData() + i * dataSize;
					memcpy(pToFill + i * vertexSize + vertexOffset, pData, dataSize);
				} 
			}
			else
			{
				_LOOPi(numNodes)
				{
					memset(pToFill + i * vertexSize + vertexOffset, 0, dataSize);				
				}
			}
		}
			
		idx++;
	}
	
	pVertexData->SetDataLength(numNodes * vertexSize);
	
	wchar buffer[256];
	wsprintf(buffer, _W("Num Vertices - Before Weld: %d"), numNodes);
	app.LogMessage(buffer);

	uint numVertices = WeldSimilarVertices(pVertexData, vertexSize, pIB);
	
	wsprintf(buffer, _W("Num Vertices - After Weld: %d"), numVertices);
	app.LogMessage(buffer);
	
	if(generateTangents || generateBinormals)
	{
        ushort* pReadOnlyIndices = pIB->Lock(0, 0);
		ComputeTangentAndBinormals(pVertexData->GetData(), numVertices, pReadOnlyIndices, pIB->GetNumIndices(), pModDesc);
        pIB->Unlock(FALSE);
	}

    CRCDataKey id;
    id.Set(pVertexData->GetData(), pVertexData->GetDataLength());

    //wstring key = obj.GetName().GetWideString();
    //key += _W("_vb_default_1");
    IRVertexBuffer* pVB = g_pRenderer->GetRResourceMgr().CreateVertexBuffer(&id, pModDesc, numVertices, BUF_DEFAULT);
    //	pVB->SetData(pVertexData);
    byte* pVBData = pVB->Lock(0, numVertices);
    memcpy(pVBData, pVertexData->GetData(), pVertexData->GetDataLength());
    pVB->Unlock();

	return pVB;
}

void
ExportMesh(XSI::X3DObject& obj, 
		   const RVertexDesc* pDesc,
		   IRVertexBufferGroup** ppVBGroup,
		   IRIndexBuffer** ppIB)
{
	//_DEBUG_ASSERT(pVBGroupBuffer);
	//_DEBUG_ASSERT(pIBBuffer);
	
	XSI::Application app;
	
	if(obj.IsValid())
	{	
		XSI::Primitive prim = obj.GetActivePrimitive();
		if(prim.IsValid())
		{
			XSI::PolygonMesh geom = prim.GetGeometry();
			if(geom.IsValid())
			{
				if(pDesc)
				{
					// Log output
					uint idx = 0;
					while(pDesc[idx].usage != VDU_END)
					{
						switch(pDesc[idx].usage)
						{
							case VDU_POSITION:		app.LogMessage(	_W("usage: VDU_POSITION") );		break;
							case VDU_NORMAL:		app.LogMessage(	_W("usage: VDU_NORMAL") );			break;
							case VDU_BLENDINDICES:	app.LogMessage(	_W("usage: VDU_BLENDINDICES") );	break;
							case VDU_BLENDWEIGHT:	app.LogMessage(	_W("usage: VDU_BLENDWEIGHT") );		break;
							case VDU_TANGENT:		app.LogMessage( _W("usage: VDU_TANGENT") );			break;
							case VDU_BINORMAL:		app.LogMessage( _W("usage: VDU_BINORMAL") );		break;	
							case VDU_COLOR:			app.LogMessage( _W("usage: VDU_COLOR") );		break;	
							case VDU_TEXCOORDF1:	app.LogMessage( _W("usage: VDU_TEXCOORDF1") );		break;	
							case VDU_TEXCOORDF2:	app.LogMessage( _W("usage: VDU_TEXCOORDF2") );		break;	
							case VDU_TEXCOORDF3:	app.LogMessage( _W("usage: VDU_TEXCOORDF3") );		break;	
							case VDU_TEXCOORDF4:	app.LogMessage( _W("usage: VDU_TEXCOORDF4") );		break;	
							default: app.LogMessage( _W("usage: UNKNOWN") );	break;	
						}

						idx++;		
					}
				
					// Extract buffers from XSI
					RVertexDesc* pModDesc = _NEW RVertexDesc[idx+1];
					IRIndexBuffer* pIB = ExtractIndices(obj);
					IRVertexBuffer* pVB = ExtractVertices(obj, pDesc, pModDesc, pIB);
					
					idx = 0;
					while(pModDesc[idx].usage != VDU_END)
					{
						switch(pModDesc[idx].usage)
						{
						case VDU_POSITION:		app.LogMessage(	_W("MOD usage: VDU_POSITION") );		break;
						case VDU_NORMAL:		app.LogMessage(	_W("MOD usage: VDU_NORMAL") );			break;
						case VDU_BLENDINDICES:	app.LogMessage(	_W("MOD usage: VDU_BLENDINDICES") );	break;
						case VDU_BLENDWEIGHT:	app.LogMessage(	_W("MOD usage: VDU_BLENDWEIGHT") );		break;
						case VDU_TANGENT:		app.LogMessage( _W("MOD usage: VDU_TANGENT") );			break;
						case VDU_BINORMAL:		app.LogMessage( _W("MOD usage: VDU_BINORMAL") );		break;	
						case VDU_COLOR:			app.LogMessage( _W("MOD usage: VDU_COLOR") );		break;	
						case VDU_TEXCOORDF1:	app.LogMessage( _W("MOD usage: VDU_TEXCOORDF1") );		break;	
						case VDU_TEXCOORDF2:	app.LogMessage( _W("MOD usage: VDU_TEXCOORDF2") );		break;	
						case VDU_TEXCOORDF3:	app.LogMessage( _W("MOD usage: VDU_TEXCOORDF3") );		break;	
						case VDU_TEXCOORDF4:	app.LogMessage( _W("MOD usage: VDU_TEXCOORDF4") );		break;	
						default: app.LogMessage( _W("MOD usage: UNKNOWN") );	break;	
						}

						idx++;		
					}

					wchar buffer[256];
					wsprintf(buffer, _W("Num Vertices: %d"), pVB->GetNumVertices());
					app.LogMessage(buffer);
					wsprintf(buffer, _W("Num Indices: %d"), pIB->GetNumIndices());
					app.LogMessage(buffer);
									
					// Save to buffers
					CRCDataKey key;
					UUID id;
					UuidCreate(&id);					
					key.Set(&id, sizeof(UUID));
					IRVertexBufferGroup* pVBGroup = g_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(&key);
					if(pVBGroup->GetNumOfVertexBuffers() == 0)
						pVBGroup->AddVertexBuffer(pVB);
					else
						wsprintf(buffer, _W("Found duplicate VB Group!!!!"));
					
					// Optimize pre/post transform vertex cache
					wsprintf(buffer, _W("Optimizing post transform cache"));
					app.LogMessage(buffer);
					RUPostTransVertCacheOpt postTrans;
					postTrans.Optimize(pIB, pVB->GetNumVertices());
	
					wsprintf(buffer, _W("Optimizing pre transform cache"));
					app.LogMessage(buffer);
					RUPreTransVertCacheOpt preTrans;
					preTrans.Optimize(pVBGroup, pIB);
		
					_DELETE_ARRAY(pModDesc);

					*ppIB = pIB;
					*ppVBGroup = pVBGroup;
				}				
			}
		}
	}
}
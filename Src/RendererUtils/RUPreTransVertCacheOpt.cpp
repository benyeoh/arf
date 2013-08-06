//==============================================================================
//
//		RUPreTransVertCacheOpt.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/15/2009
//
//==============================================================================

#include "RendererUtils.h"

_NAMESPACE_BEGIN

void	
RUPreTransVertCacheOpt::Optimize(IRVertexBufferGroup* pToOptimize, IRIndexBuffer* pIB)
{
	uint numVertices = pToOptimize->GetVertexBuffer(0)->GetNumVertices();

	DenseChainedHashMap<uint, uint, NullUintHash> vertexRemap;
	vertexRemap.Initialize(numVertices * 2, 0.9f);
	
	IByteBufferPtr pBackupBuffers[256];	
	byte* pSrcData[256];
	byte* pDestData[256];
	uint numVertexBuffers = pToOptimize->GetNumOfVertexBuffers();
	
	_LOOPi(numVertexBuffers)
	{
		IRVertexBuffer* pVB = pToOptimize->GetVertexBuffer(i);
		uint vertexSize = pVB->GetVertexSize();
		pBackupBuffers[i] = _NEW CByteBuffer(vertexSize * numVertices);			
		pSrcData[i] = pBackupBuffers[i]->GetData();
		pDestData[i] = pVB->Lock(pVB->GetStartVertex(), numVertices);
		memcpy(pSrcData[i], pDestData[i], vertexSize * numVertices);
	}
	
	uint currentVertex = 0;	
	uint numIndices = pIB->GetNumIndices();
	ushort* pIndices = pIB->Lock(pIB->GetStartIndex(), numIndices);
	_LOOPi(numIndices)
	{
		uint* pRemap = vertexRemap.Get(pIndices[i]);
		if(!pRemap)
		{
			// If not remapped then append it to the current end of
			// the vb
			vertexRemap.Add(pIndices[i], currentVertex);
			
			_LOOPj(numVertexBuffers)
			{
				uint vertexSize = pToOptimize->GetVertexBuffer(j)->GetVertexSize();
				memcpy(pDestData[j] + currentVertex * vertexSize, 
					pSrcData[j] + pIndices[i] * vertexSize, vertexSize);
			}

			pIndices[i] = currentVertex;
			currentVertex++;
		}
		else
		{
			pIndices[i] = *pRemap;
		}
	}
	
	pIB->Unlock();
	_LOOPi(numVertexBuffers)
	{
		pToOptimize->GetVertexBuffer(i)->Unlock();
	}
	
}

_NAMESPACE_END
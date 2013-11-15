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
	
	IByteBufferPtr pBackupBuffers[32];	
	byte* pSrcData[32];
	byte* pDestData[32];
	uint numVertexBuffers = pToOptimize->GetNumOfVertexBuffers();
	_DEBUG_ASSERT(numVertexBuffers <= 32);

	_LOOPi(numVertexBuffers)
	{
		IRVertexBuffer* pVB = pToOptimize->GetVertexBuffer(i);
		uint vertexSize = pVB->GetVertexSize();
		pBackupBuffers[i] = _NEW CByteBuffer(vertexSize * numVertices);			
		pSrcData[i] = pBackupBuffers[i]->GetData();

		byte* pReadOnlyVBData = pVB->Lock(pVB->GetStartVertex(), 0);
		memcpy(pSrcData[i], pReadOnlyVBData, vertexSize * numVertices);
		pVB->Unlock(FALSE);

		pDestData[i] = pVB->Lock(pVB->GetStartVertex(), numVertices);
	}
	
	uint currentVertex = 0;	
	uint numIndices = pIB->GetNumIndices();
	IByteBufferPtr pReadIB = _NEW CByteBuffer(numIndices * sizeof(ushort));
	ushort* pReadIndices = pIB->Lock(pIB->GetStartIndex(), 0);
	memcpy(pReadIB->GetData(), pReadIndices, sizeof(ushort) * numIndices);
	pIB->Unlock(FALSE);

	ushort* pIndices = (ushort*) pReadIB->GetData();
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

	ushort* pIndicesWrite = pIB->Lock(pIB->GetStartIndex(), numIndices);
	memcpy(pIndicesWrite, pIndices, sizeof(ushort) * numIndices);
	pIB->Unlock();

	_LOOPi(numVertexBuffers)
	{
		pToOptimize->GetVertexBuffer(i)->Unlock();
	}
	
}

_NAMESPACE_END
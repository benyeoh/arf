//==============================================================================
//
//		CRVertexBuffer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 26, 2006
//
//==============================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

const RVertexDesc* 
CRVertexBuffer::GetDescription() const 
{
	return m_pVertexDesc;
}


byte* 
CRVertexBuffer::LockImmediate(uint startVertexToWrite, uint numVerticesToWrite)
{
	byte* pData = DoLockImmediate(startVertexToWrite, numVerticesToWrite);

	if(pData)
	{
		m_TempStartVertex = startVertexToWrite;
		m_TempNumVertices = numVerticesToWrite;
	}
	
	return pData;
}

boolean 
CRVertexBuffer::UnlockImmediate(boolean isSetValidRange)
{
	if(DoUnlockImmediate())
	{
		if(isSetValidRange && m_TempNumVertices > 0)
		{
			SetValidRange(m_TempStartVertex, m_TempNumVertices);
		}

		return TRUE;
	}

	return FALSE;
}	
	
byte*
CRVertexBuffer::Lock(uint startVertexToWrite, uint numVerticesToWrite)
{
	byte* pData = DoLock(startVertexToWrite, numVerticesToWrite);

	if(pData)
	{
		m_TempStartVertex = startVertexToWrite;
		m_TempNumVertices = numVerticesToWrite;
	}

	return pData;
}

boolean
CRVertexBuffer::Unlock(boolean isSetValidRange)
{
	if(DoUnlock())
	{
		if(isSetValidRange && m_TempNumVertices > 0)
		{
			SetValidRange(m_TempStartVertex, m_TempNumVertices);
		}

		return TRUE;
	}

	return FALSE;
}

void 
CRVertexBuffer::SetDescription(const RVertexDesc* pDesc)
{
	_DEBUG_ASSERT(pDesc);

	uint vertexSize = 0;

	uint i = 0;
	while( pDesc[i].usage != VDU_END )
	{
		vertexSize += GetUsageSize(pDesc[i].usage);		
		m_pVertexDesc[i] = pDesc[i];		
		++i;
	}
	
	m_pVertexDesc[i].usage = VDU_END;
	m_pVertexDesc[i].usageIndex = 0;	
	
	m_VertexSize = vertexSize;
}

void 
CRVertexBuffer::SetValidRange(uint startVertex, uint numVertices)
{
	SetStartVertex(startVertex);
	SetNumVertices(numVertices);
}
	
void 
CRVertexBuffer::Initialize(const RVertexDesc* pDesc,
						uint numVertices,
						eRBufferUsage usage)
{
	SetDescription(pDesc);
	SetUsage(usage);
	SetSize(numVertices);
	SetValidRange(0, 0);
}
				
_NAMESPACE_END
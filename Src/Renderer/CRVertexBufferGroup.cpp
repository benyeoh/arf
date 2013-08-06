//==============================================================================
//
//		CRVertexBufferGroup.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 23, 2006
//
//==============================================================================

#include "RendererFramework.h"



_NAMESPACE_BEGIN

void 
CRVertexBufferGroup::AddVertexBuffer(IRVertexBuffer* pVB)
{
	_DEBUG_ASSERT(pVB);
	
	CRVertexBuffer* pCVB = (CRVertexBuffer*)(pVB);
	m_VBs.push_back( pCVB );
	DoNotifyAddedVertexBuffer(pCVB);
}

void 
CRVertexBufferGroup::RemoveVertexBuffer(uint index)
{
	_DEBUG_ASSERT(index >= 0);
	_DEBUG_ASSERT(index < m_VBs.size());
	
	CRVertexBuffer* pCVB  = m_VBs[index];
	pCVB->AddRef();

	m_VBs.erase(m_VBs.begin()+index);	
	DoNotifyRemovedVertexBuffer(pCVB);

	pCVB->Release();
}

IRVertexBuffer* 
CRVertexBufferGroup::GetVertexBuffer(uint index)
{
	_DEBUG_ASSERT(index >= 0);
	_DEBUG_ASSERT(index < m_VBs.size());
	
	return m_VBs[index];
}

uint 
CRVertexBufferGroup::GetNumOfVertexBuffers()
{
	return (uint)(m_VBs.size());
}


_NAMESPACE_END
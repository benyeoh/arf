//==============================================================================
//
//		CRVertexBufferGroup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 23, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRVertexBufferGroup : public CRResource< IRVertexBufferGroup >
{
protected:
	std::vector< CRVertexBufferPtr > m_VBs;	// Keep strong pointers to the vbs
	CRRenderer* m_pRenderer;

public:
	CRVertexBufferGroup(CRRenderer* pRenderer)
		: m_pRenderer(pRenderer)
	{
	}
	
	virtual ~CRVertexBufferGroup()
	{
	}
	
protected:
	virtual void DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB) = 0;
	virtual void DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB) = 0;
	
public:
	void AddVertexBuffer(IRVertexBuffer* pVB);
	void RemoveVertexBuffer(uint index);
	IRVertexBuffer* GetVertexBuffer(uint index); 
	uint GetNumOfVertexBuffers();
};

_DECLARE_SMARTPTR(CRVertexBufferGroup)

_NAMESPACE_END
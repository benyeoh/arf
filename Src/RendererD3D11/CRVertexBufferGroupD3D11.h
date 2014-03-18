//==============================================================================
//
//		CRVertexBufferGroupD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 9 March, 2014
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRVertexBufferGroupD3D11 : public CRVertexBufferGroup
{
private:

public:
	CRVertexBufferGroupD3D11(CRRenderer* pRenderer)
		: CRVertexBufferGroup(pRenderer)
		, m_pInstancedVB(NULL)
		, m_pInstancedDecl(NULL)
	{
		m_CachedInstancedVBIndices.Initialize(32, 0.7f);
		m_CachedInstancedDeclIndices.Initialize(32, 0.7f);
	}

	virtual ~CRVertexBufferGroupD3D()
	{
	}


protected:
	void DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB);
	void DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB);	

public:

};

_NAMESPACE_END

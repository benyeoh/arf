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
	DenseChainedHashMap<size_t, ID3D11InputLayout*, NullSizeTHash> m_InputLayoutMap;

	D3D11_INPUT_ELEMENT_DESC m_InputDesc[MAX_VERTEX_DESC];
	uint m_NumElems;

public:
	CRVertexBufferGroupD3D11(CRRenderer* pRenderer)
		: CRVertexBufferGroup(pRenderer)
		, m_NumElems(0)
	{
		m_InputLayoutMap.Initialize(7, 0.67f);
	}

	virtual ~CRVertexBufferGroupD3D()
	{
		ResetCache();
	}

private:
	void ResetCache();
	void CacheInputLayout();

protected:
	void DoNotifyAddedVertexBuffer(CRVertexBuffer* pVB);
	void DoNotifyRemovedVertexBuffer(CRVertexBuffer* pVB);	

public:
	uint GetNumElems() { return m_NumElems; }
	D3D11_INPUT_ELEMENT_DESC* GetInputElems() { return m_InputDesc; }

	void AddInputLayout(ID3D11InputLayout* pInputLayout, ID3D11VertexShader* pKey);
	ID3D11InputLayout* GetInputLayout(ID3D11VertexShader* pVS);


};

_NAMESPACE_END

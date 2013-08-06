//==============================================================================
//
//		CRVertexBufferD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 13 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRVertexBufferD3D11 : public CRVertexBuffer
{
private:
	D3D11_BUFFER_DESC m_Desc;

	CRVertexBufferD3D11* m_pParent;

	ID3D11BufferPtr m_pD3DVB;
	IByteBufferPtr m_pOffScreen;

	void*	m_pLockData;

public:
	CRVertexBufferD3D11(CRRenderer* pRenderer)
		: CRVertexBuffer(pRenderer)
		, m_pLockData(NULL)
		, m_pParent(NULL)
	{
	}

	virtual ~CRVertexBufferD3D11()
	{
	}

protected:
	byte* DoLock(uint startVertexToWrite, uint numVerticesToWrite);
	boolean DoUnlock();
	byte* DoLockImmediate(uint startVertexToWrite, uint numVerticesToWrite);
	boolean DoUnlockImmediate();

	//boolean DoSetData(IByteBuffer* pData);

public:
	void SetD3DVB(IDirect3DVertexBuffer9* pD3DVB, IByteBuffer* pOffScreen);
	void SetD3DSharedVB(CRVertexBufferD3D* pParent);

	IByteBuffer* GetOffscreenVB() { return m_pOffScreen; }
	ID3D11Buffer* GetD3DVB() { return m_pD3DVB; }
};

_NAMESPACE_END
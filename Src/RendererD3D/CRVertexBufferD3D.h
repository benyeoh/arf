//==============================================================================
//
//		CRVertexBufferD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, April 11, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRVertexBufferD3D : public CRVertexBuffer
{
private:
	CRVertexBufferD3D* m_pParent;

	IDirect3DVertexBuffer9Ptr m_pD3DVB;
	IByteBufferPtr m_pOffScreen;

	void*	m_pLockData;
		
public:
	CRVertexBufferD3D(CRRenderer* pRenderer)
		: CRVertexBuffer(pRenderer)
		, m_pLockData(NULL)
		, m_pParent(NULL)
	{
	}
	
	virtual ~CRVertexBufferD3D()
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

	void OnLostDevice();
	void OnResetDevice();
	void OnRestoreDevice();
	
	IByteBuffer* GetOffscreenVB() { return m_pOffScreen; }
	IDirect3DVertexBuffer9* GetD3DVB() { return m_pD3DVB; }
};

_NAMESPACE_END

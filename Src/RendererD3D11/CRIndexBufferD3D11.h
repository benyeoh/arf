//==============================================================================
//
//		CRIndexBufferD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 13 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRIndexBufferD3D11 : public CRIndexBuffer
{
private:
	D3D11_BUFFER_DESC m_Desc;

	CRIndexBufferD3D11* m_pParent;

	ID3D11BufferPtr m_pD3DIB;
	ID3D11BufferPtr m_pOffScreen;

	void*	m_pLockData;
	ScratchPad* m_pScratchPad;

public:
	CRIndexBufferD3D11(CRRenderer* pRenderer)
		: CRIndexBuffer(pRenderer)
		, m_pLockData(NULL)
		, m_pParent(NULL)
		, m_pScratchPad(NULL)
	{
	}

	virtual ~CRIndexBufferD3D11()
	{
	}

protected:	
	ushort* DoLock(uint startIndexToWrite, uint numIndicesToWrite);
	boolean DoUnlock();

	//boolean DoSetData(IByteBuffer* pData);

public:
	void SetD3DIB(ID3D11Buffer* pD3DIB, IByteBuffer* pOffScreen);
	void SetD3DSharedIB(CRIndexBufferD3D11* pParent);

	ID3D11Buffer* GetOffscreenIB() { return m_pOffScreen; }
	ID3D11Buffer* GetD3DIB() { return m_pD3DIB; }
};

_NAMESPACE_END
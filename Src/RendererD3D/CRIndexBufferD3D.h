//==============================================================================
//
//		CRIndexBufferD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, April 24, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRIndexBufferD3D : public CRIndexBuffer
{
private:
	CRIndexBufferD3D* m_pParent;

	IDirect3DIndexBuffer9Ptr m_pD3DIB;
	IDirect3DIndexBuffer9Ptr m_pOffScreen;
		 	
	void* m_pLockData;
	
public:
	CRIndexBufferD3D(CRRenderer* pRenderer)
		: CRIndexBuffer(pRenderer)
		, m_pLockData(NULL)
		, m_pParent(NULL)
	{
	}

	virtual ~CRIndexBufferD3D()
	{	
	}
	
protected:
	ushort* DoLock(uint startIndexToWrite, uint numIndicesToWrite);
	boolean DoUnlock();
	
public:
	void OnLostDevice();
	void OnResetDevice();
	void OnRestoreDevice();
	
	void SetD3DSharedIB(CRIndexBufferD3D* pParent);
	void SetD3DIB(IDirect3DIndexBuffer9* pD3DIB, IDirect3DIndexBuffer9* pOffScreen);
	
	IDirect3DIndexBuffer9* GetOffscreenIB() { return m_pOffScreen; }
	IDirect3DIndexBuffer9* GetD3DIB() { return m_pD3DIB; }
};

_NAMESPACE_END

//==============================================================================
//
//		CRDepthBufferD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/25/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRDepthBufferD3D : public CRDepthBuffer
{
private: 
	IDirect3DSurface9Ptr m_pDepthStencil;
	boolean m_IsBackBufferDepth;

public: 
	CRDepthBufferD3D()
		: m_IsBackBufferDepth(false)
	{
	}

	virtual ~CRDepthBufferD3D()
	{
	}

public:
	void SetDepthStencilSurface(IDirect3DSurface9* pDepthStencil, boolean isBackBufferDepth);
	
	IDirect3DSurface9* GetDepthStencilSurface();
	
	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();
};

_NAMESPACE_END

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

class CRDepthBufferD3D11 : public CRDepthBuffer
{
private: 
	CRTexture2DD3D11Ptr m_pDepthTexture; 

public: 
	CRDepthBufferD3D11()
	{
	}

	virtual ~CRDepthBufferD3D11()
	{
	}

public:
	void SetD3D11Texture(CRTexture2DD3D11* pTex) { m_pDepthTexture = pTex; }
	CRTexture2DD3D11* GetD3D11Texture() { return m_pDepthTexture; }

	void OnResetDevice() {}
	void OnLostDevice() {}
	void OnRestoreDevice() {}
};

_NAMESPACE_END

//==============================================================================
//
//		CRTextureD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTextureD3D : public CRTexture
{
private:
	IDirect3DTexture9Ptr m_pD3DTexture;
//	boolean m_IsManaged;

public: 
	CRTextureD3D(CRRenderer* pRenderer)
		: CRTexture(pRenderer)
	{
	}
	
	virtual ~CRTextureD3D()
	{
	}
		
public:
	IDirect3DTexture9* GetD3DTexture() { return m_pD3DTexture; }

	void SetD3DTexture(IDirect3DTexture9* pTex);
//	void SetD3DDescription(boolean isManaged);
	
	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();

};

_NAMESPACE_END

//==============================================================================
//
//		CRTextureRTD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/23/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTextureRTD3D : public CRTextureRT
{
private: 
	//IDirect3DTexture9Ptr m_pD3DTexture;
	//std::vector<IDirect3DSurface9Ptr> m_pD3DTextureSurfaces;

	//std::vector<D3DSURFACE_DESC> m_Desc;
	D3DSURFACE_DESC m_Desc;
	//boolean m_IsBB;

	IDirect3DSurface9Ptr m_pD3DSurface;
	IDirect3DSurface9Ptr m_pD3DReadback;

public: 
	CRTextureRTD3D(CRRenderer* pRenderer)
		: CRTextureRT(pRenderer)
		//, m_IsBB(FALSE)
	{
	}

	virtual ~CRTextureRTD3D()
	{
	}

protected:
	uint DoGetWidth() const;
	uint DoGetHeight() const;
	uint DoGetMipMapLevels() const;
    void DoCopyFromTextureRT(IRTextureRT* pRT);

	boolean DoLockReadOnlyPrepare();
	byte* DoLockReadOnly(uint& pitch);
	boolean DoUnlockReadOnly();

public: 
	//IDirect3DTexture9* GetD3DTexture()	{ return m_pD3DTexture; }
	//void SetD3DTexture(IDirect3DTexture9* pTex);

	void SetD3DSurfaceOnly(IDirect3DSurface9* pSurface);
	IDirect3DSurface9* GetD3DSurface();

	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();
};

_DECLARE_SMARTPTR(CRTextureRTD3D)

_NAMESPACE_END

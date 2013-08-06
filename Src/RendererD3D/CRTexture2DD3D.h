//==============================================================================
//
//		CRTexture2DD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTexture2DD3D : public CRTexture2D
{
private:
	IDirect3DTexture9Ptr m_pD3DTexture;
	IDirect3DTexture9Ptr m_pOffScreen;
	std::vector<IDirect3DSurface9Ptr> m_pOffScreenSurfaces;
	std::vector<IDirect3DSurface9Ptr> m_pD3DTextureSurfaces;
		
	std::vector<D3DSURFACE_DESC> m_Desc;
	std::vector<RECT> m_DirtyRegions;
	
	std::vector<CRTextureRTD3DPtr> m_pRT;

public: 
	CRTexture2DD3D(CRRenderer* pRenderer)
		: CRTexture2D(pRenderer)
	{
	}
	
	virtual ~CRTexture2DD3D()
	{
	}

protected:
	boolean DoUnlock(uint level);
	byte* DoLock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite);

	byte* DoLockImmediate(uint level, uint& pitch);
	boolean DoUnlockImmediate(uint level);

	uint DoGetMipMapLevels() const;
	uint DoGetWidth(uint level) const;
	uint DoGetHeight(uint level) const;
	
	CRTextureRT* DoGetTextureRT(uint level);

public:
	IDirect3DTexture9* GetD3DTexture() { return m_pD3DTexture; }
	IDirect3DTexture9* GetD3DOffscreen() { return m_pOffScreen; }

	void SetD3DTexture(IDirect3DTexture9* pTex, IDirect3DTexture9* pOffScreen);
	void SetD3DTextureRT(IDirect3DTexture9* pTex);

	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();
};

_NAMESPACE_END

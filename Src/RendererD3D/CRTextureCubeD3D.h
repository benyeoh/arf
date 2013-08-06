//==============================================================================
//
//		CRTextureCubeD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/14/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTextureCubeD3D : public CRTextureCube
{
private:
	IDirect3DCubeTexture9Ptr m_pD3DTexture;
	IDirect3DCubeTexture9Ptr m_pOffScreen;
	std::vector<IDirect3DSurface9Ptr> m_pOffScreenSurfaces[CF_NUM_FACES];
	std::vector<IDirect3DSurface9Ptr> m_pD3DTextureSurfaces[CF_NUM_FACES];

	std::vector<D3DSURFACE_DESC> m_Desc;
	std::vector<RECT> m_DirtyRegions[CF_NUM_FACES];

	std::vector<CRTextureRTD3DPtr> m_pCubeFaceRT[CF_NUM_FACES];

public: 
	CRTextureCubeD3D(CRRenderer* pRenderer)
		: CRTextureCube(pRenderer)
	{
	}

	virtual ~CRTextureCubeD3D()
	{
	}

protected:
	boolean DoUnlock(uint level, eRCubeFace face);
	byte* DoLock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite);

	uint DoGetMipMapLevels() const;
	uint DoGetSize(uint level) const;

	CRTextureRT* DoGetTextureRT(uint mipLevel, eRCubeFace face);

public:
	IDirect3DCubeTexture9* GetD3DTexture() { return m_pD3DTexture; }

	void SetD3DTexture(IDirect3DCubeTexture9* pTex, IDirect3DCubeTexture9* pOffScreen);
	void SetD3DTextureRT(IDirect3DCubeTexture9* pTex);

	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();
};

_NAMESPACE_END

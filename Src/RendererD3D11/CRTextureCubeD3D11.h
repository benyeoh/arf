//==============================================================================
//
//		CRTextureCubeD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 13 October, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTextureCubeD3D11 : public CRTextureCube
{
private:
	D3D11_TEXTURE2D_DESC m_Desc;
	ID3D11Texture2DPtr m_pD3DTexture;
	ID3D11Texture2DPtr m_pOffscreen;

	ID3D11ShaderResourceViewPtr m_pSRV;
	DenseChainedHashMap<uint, CRTextureRTD3D11Ptr, NullUintHash> m_CubeFaceRTTable;

	ScratchPad* m_pLockPad;
	gmtl::Vec4i m_WriteOffset;
	D3D11_SUBRESOURCE_DATA	m_OffscreenData;

public: 
	CRTextureCubeD3D11(CRRenderer* pRenderer)
		: CRTextureCube(pRenderer)
	{
	}

	virtual ~CRTextureCubeD3D11()
	{
	}

protected:
	boolean DoUnlock(uint level, eRCubeFace face);
	byte* DoLock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite);

	uint DoGetMipMapLevels() const;
	uint DoGetSize(uint level) const;

	CRTextureRT* DoGetTextureRT(uint mipLevel, eRCubeFace face);

public:
	ID3D11Texture2D* GetD3DTexture() { return m_pD3DTexture; }

	void SetD3DTexture(ID3D11Texture2D* pTex, ID3D11Texture2D* pOffScreen);
	void SetD3DTextureRT(ID3D11Texture2D* pTex);
};

_NAMESPACE_END

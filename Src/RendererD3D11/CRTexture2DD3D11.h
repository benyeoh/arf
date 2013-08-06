//==============================================================================
//
//		CRTexture2DD3D11.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTexture2DD3D11 : public CRTexture2D
{
private:
	D3D11_TEXTURE2D_DESC m_Desc;
	ID3D11Texture2DPtr m_pD3DTexture;
	ID3D11Texture2DPtr m_pOffscreen;

	ID3D11ShaderResourceViewPtr m_pSRV;
	std::vector<CRTextureRTD3D11Ptr> m_pRT;

	ScratchPad* m_pLockPad;
	gmtl::Vec4i m_WriteOffset;
	D3D11_SUBRESOURCE_DATA	m_OffscreenData;
	//std::vector<CRTextureRTD3D11Ptr> m_pRT;

public: 
	CRTexture2DD3D11(CRRenderer* pRenderer)
		: CRTexture2D(pRenderer)
		, m_pLockPad(NULL)
	{
		m_OffscreenData.pSysMem = NULL;
		m_OffscreenData.SysMemPitch = 0;
	}

	virtual ~CRTexture2DD3D()
	{
		_DEBUG_ASSERT(m_pLockPad == NULL);
		_DEBUG_ASSERT(m_OffscreenData.pSysMem == NULL);
	}

private:
	byte* MapDirectResource(uint level, uint& pitch);
	void UnmapDirectResource(uint level);
	byte* MapProxyResource(uint level, uint& pitch);
	void UnmapProxyResource(uint level);

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
	ID3D11Texture2D* GetD3DTexture() { return m_pD3DTexture; }
	ID3D11Texture2D* GetD3DOffscreen() { return m_pOffScreen; }
	ID3D11ShaderResourceView* GetSRV() { return m_pSRV; }

	void SetD3DTexture(ID3D11Texture2D* pTex, ID3D11Texture2D* pOffScreen);
	//void SetD3DTextureRT(IDirect3DTexture9* pTex);
};

_NAMESPACE_END

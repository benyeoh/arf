//==============================================================================
//
//		CRTexture3DD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Tuesday, 31 December, 2013
//
//==============================================================================


#pragma once

_NAMESPACE_BEGIN

class CRTexture3DD3D11 : public CRTexture3D
{
private:
	D3D11_TEXTURE3D_DESC m_Desc;
	ID3D11Texture3DPtr m_pD3DTexture;
	ID3D11Texture3DPtr m_pOffscreen;

	ID3D11ShaderResourceViewPtr m_pSRV;
	DenseChainedHashMap<uint, CRTextureRTD3D11Ptr, NullUintHash> m_RTTable;

	ScratchPad* m_pLockPad;
	gmtl::Vec3i m_WriteStart;
	gmtl::Vec3i m_WriteEnd;
	D3D11_MAPPED_SUBRESOURCE m_OffscreenData;

public: 
	CRTexture3DD3D11(CRRenderer* pRenderer)
		: CRTexture3D(pRenderer)
		, m_pLockPad(NULL)
	{
		m_OffscreenData.DepthPitch = 0;
		m_OffscreenData.pData = NULL;
		m_OffscreenData.RowPitch = 0;
	}

	virtual ~CRTextureCubeD3D11()
	{
		_DEBUG_ASSERT(m_OffscreenData.pData == NULL);
	}

private:
	byte* MapDirectResource(uint level, uint& rowPitch, uint& slicePitch);
	void UnmapDirectResource(uint level);
	byte* MapProxyResource(uint level, uint& rowPitch, uint& slicePitch);
	void UnmapProxyResource(uint level);

protected:
	//byte* DoLockImmediate(uint level, eRCubeFace face, uint& pitch);
	//boolean DoUnlockImmediate(uint level, eRCubeFace face);

	CRTextureRT* DoGetTextureRT(uint mipLevel, uint startZSlice, uint numZSlices);

	byte* DoLock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd);
	boolean DoUnlock(uint level);

	uint DoGetWidth(uint level) const;
	uint DoGetHeight(uint level) const;
	uint DoGetDepth(uint level) const;

	byte* DoLockImmediate(uint level, uint& rowPitch, uint& slicePitch);
	boolean DoUnlockImmediate(uint level);

	uint DoGetMipMapLevels() const;

public:
	ID3D11Texture3D* GetD3DTexture() { return m_pD3DTexture; }

	void SetD3DTexture(ID3D11Texture3D* pTex, ID3D11Texture3D* pOffScreen);
	void SetD3DTextureRT(ID3D11Texture3D* pTex);
};

_NAMESPACE_END


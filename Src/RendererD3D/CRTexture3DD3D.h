//================================================================================
//
//		CRTexture3DD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/24/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTexture3DD3D : public CRTexture3D
{
private:
	IDirect3DVolumeTexture9Ptr m_pD3DTexture;
	IDirect3DVolumeTexture9Ptr m_pOffScreen;
	std::vector<IDirect3DVolume9Ptr> m_pOffScreenSurfaces;
	std::vector<IDirect3DVolume9Ptr> m_pD3DTextureSurfaces;

	std::vector<D3DVOLUME_DESC> m_Desc;
	std::vector<D3DBOX> m_DirtyRegions;

public: 
	CRTexture3DD3D(CRRenderer* pRenderer)
		: CRTexture3D(pRenderer)
	{
	}

	virtual ~CRTexture3DD3D()
	{
	}

protected:
	byte* DoLock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd);
	boolean DoUnlock(uint level);

	uint DoGetWidth(uint level) const;
	uint DoGetHeight(uint level) const;
	uint DoGetDepth(uint level) const;

	byte* DoLockImmediate(uint level, uint& rowPitch, uint& slicePitch);
	boolean DoUnlockImmediate(uint level);

	uint DoGetMipMapLevels() const;

public:
	IDirect3DVolumeTexture9* GetD3DTexture() { return m_pD3DTexture; }
	IDirect3DVolumeTexture9* GetD3DOffscreen() { return m_pOffScreen; }

	void SetD3DTexture(IDirect3DVolumeTexture9* pTex, IDirect3DVolumeTexture9* pOffScreen);

	void OnResetDevice();
	void OnLostDevice();
	void OnRestoreDevice();
};

_NAMESPACE_END

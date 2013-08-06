//==============================================================================
//
//		CRTextureRTD3D11.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 3 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRTextureRTD3D11 : public CRTextureRT
{
private:
	ID3D11RenderTargetViewPtr m_pRTView;
	uint m_Width;
	uint m_Height;

public:
	CRTextureRTD3D11(CRRenderer* pRenderer)
		: CRTextureRT(pRenderer)
	{

	}

	virtual ~CRTextureRTD3D11()
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
	//void SetD3DTexture(IDirect3DTexture9* pTex);i

	void SetRTV(ID3D11RenderTargetView* pRTV, uint width, uint height);
	ID3D11RenderTargetView* GetRTV();
};

_DECLARE_SMARTPTR(CRTextureRTD3D11)

_NAMESPACE_END
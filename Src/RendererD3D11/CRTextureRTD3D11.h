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

	D3D11_RESOURCE_DIMENSION m_ResType;
	ID3D11ResourcePtr m_pResource;
	ID3D11ResourcePtr m_pOffscreen;
	
	D3D11_RENDER_TARGET_VIEW_DESC m_Desc;

	uint m_Width;
	uint m_Height;
	uint m_MipSlice;
	uint m_MipLevels;
	uint m_StartArray;
	uint m_ArraySize;

	uint m_Depth;
	uint m_StartW;
	uint m_WSize;

	DXGI_FORMAT m_Format;

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

	void SetRTV(ID3D11Resource* pRes, ID3D11RenderTargetView* pRTV);
	ID3D11RenderTargetView* GetRTV();

	ID3D11Resource* GetD3D11Resource() { return m_pResource; }

};

_DECLARE_SMARTPTR(CRTextureRTD3D11)

_NAMESPACE_END
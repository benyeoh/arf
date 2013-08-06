//==============================================================================
//
//		CRTextureRTD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/23/2007
//
//==============================================================================

#include "RendererD3D.h"

_NAMESPACE_BEGIN

uint 
CRTextureRTD3D::DoGetWidth() const
{
	return m_Desc.Width;
}

uint 
CRTextureRTD3D::DoGetHeight() const
{
	return m_Desc.Height;
}

uint 
CRTextureRTD3D::DoGetMipMapLevels() const
{
	return (uint) 1;
}

//void 
//CRTextureRTD3D::SetD3DTexture(IDirect3DTexture9* pTex)
//{
//	_DEBUG_ASSERT(pTex);
//
//	m_IsSurfaceOnly = false;
//	m_pD3DTexture = pTex;
//	m_pD3DTextureSurfaces.clear();
//
//	m_Desc.clear();
//
//	uint mipMapCount = m_pD3DTexture->GetLevelCount();
//	_LOOPi(mipMapCount)
//	{
//		D3DSURFACE_DESC desc;
//		m_pD3DTexture->GetLevelDesc(i, &desc);
//		m_Desc.push_back(desc);
//		IDirect3DSurface9* pD3DSurface = NULL;
//		HRESULT hr = m_pD3DTexture->GetSurfaceLevel(i, &pD3DSurface);
//		_DEBUG_ASSERT(SUCCEEDED(hr));
//		m_pD3DTextureSurfaces.push_back(pD3DSurface);
//		_RELEASE(pD3DSurface);
//	}
//}

void CRTextureRTD3D::SetD3DSurfaceOnly(IDirect3DSurface9* pSurface)
{
	_DEBUG_ASSERT(pSurface);

	//m_IsBB = true;
	//m_pD3DTexture = NULL;
	//m_pD3DTextureSurfaces.clear();

	//m_Desc.clear();

	//m_pD3DTextureSurfaces.push_back(pSurface);

	m_pD3DSurface = pSurface;

	//D3DSURFACE_DESC desc;
	pSurface->GetDesc(&m_Desc);
	//m_Desc.push_back(desc);
}

void 
CRTextureRTD3D::DoCopyFromTextureRT(IRTextureRT* pRT)
{
    IDirect3DSurface9* pBBSurface = NULL;
    CRTextureRTD3D* pD3DRT = (CRTextureRTD3D*)pRT;

    IDirect3DSurface9* pSrc = pD3DRT->GetD3DSurface();
    IDirect3DSurface9* pDest = GetD3DSurface();

    HRESULT hr = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice()->StretchRect(pSrc, NULL, pDest, NULL, D3DTEXF_POINT);
    _DEBUG_ASSERT(SUCCEEDED(hr));
}

IDirect3DSurface9* CRTextureRTD3D::GetD3DSurface()
{
	return m_pD3DSurface;
}


boolean CRTextureRTD3D::DoLockReadOnlyPrepare()
{
	CRRendererD3D* pD3DRenderer = (CRRendererD3D*) m_pRenderer;
	if(!m_pD3DReadback)
	{
		IDirect3DSurface9* pOffscreen;
		HRESULT hr = pD3DRenderer->GetD3DDevice()->CreateOffscreenPlainSurface(m_Desc.Width, m_Desc.Height, m_Desc.Format, D3DPOOL_SYSTEMMEM, &pOffscreen, NULL);
		_DEBUG_ASSERT(SUCCEEDED(hr));

		m_pD3DReadback = pOffscreen;
		pOffscreen->Release();
	}

	HRESULT hr = pD3DRenderer->GetD3DDevice()->GetRenderTargetData(GetD3DSurface(), m_pD3DReadback);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	return SUCCEEDED(hr);
}

byte* CRTextureRTD3D::DoLockReadOnly(uint& pitch)
{
	if(!m_pD3DReadback)
		return NULL;

	DWORD flags = D3DLOCK_READONLY;

	D3DLOCKED_RECT lockedRect;
	HRESULT hr = m_pD3DReadback->LockRect(&lockedRect, NULL, flags);
	_DEBUG_ASSERT(hr == D3D_OK);

	pitch = lockedRect.Pitch;

	return (byte*) lockedRect.pBits;
}

boolean CRTextureRTD3D::DoUnlockReadOnly()
{
	if(!m_pD3DReadback)
		return FALSE;

	HRESULT hr = m_pD3DReadback->UnlockRect();
	_DEBUG_ASSERT(SUCCEEDED(hr));

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}

void 
CRTextureRTD3D::OnResetDevice()
{
	//if(m_IsBB)
	{
		_DEBUG_TRACE(_W("Resetting Back buffer: %s\n"), GetResourceName());
		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetBackBufferColor(this);
		_DEBUG_ASSERT(success);
	}	
	//else
	//{
	//	_DEBUG_TRACE(_W("Resetting RT: %s\n"), GetResourceName());
	//	boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetTextureRT(this);
	//	_DEBUG_ASSERT(success);
	//}
}

void 
CRTextureRTD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost RT: %s\n"), GetResourceName());

	//m_pD3DTexture = NULL;
	//m_pD3DTextureSurfaces.clear();

	m_pD3DSurface = NULL;
}

void 
CRTextureRTD3D::OnRestoreDevice()
{
}

_NAMESPACE_END
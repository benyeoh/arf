//==============================================================================
//
//		CRTextureRTD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 3 June, 2013
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

uint CRTextureRTD3D11::DoGetWidth() const
{
	return m_Width;
}

uint CRTextureRTD3D11::DoGetHeight() const
{
	return m_Height;
}

uint CRTextureRTD3D11::DoGetMipMapLevels() const
{
	return 1;
}

void CRTextureRTD3D11::DoCopyFromTextureRT(IRTextureRT* pRT)
{
	// TODO:
}

boolean CRTextureRTD3D11::DoLockReadOnlyPrepare()
{
	// TODO:
}

byte* CRTextureRTD3D11::DoLockReadOnly(uint& pitch)
{
	// TODO:
}

boolean CRTextureRTD3D11::DoUnlockReadOnly()
{
	// TODO:
}

void CRTextureRTD3D11::SetRTV(ID3D11RenderTargetView* pRTV, uint width, uint height)
{
	m_pRTView = pRTV;
	m_Width = width;
	m_Height = height;
}

ID3D11RenderTargetView* CRTextureRTD3D11::GetRTV()
{
	return m_pRTView;
}


_NAMESPACE_END
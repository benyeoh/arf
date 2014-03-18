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
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	CRTextureRTD3D11* pRTD3D = (CRTextureRTD3D11*) pRT;

#ifdef _DEBUG
	D3D11_RESOURCE_DIMENSION toCheck;
	pRTD3D->GetD3D11Resource()->GetType(&toCheck);
	_DEBUG_ASSERT(toCheck == m_ResType);

	// TODO: Do additional checks here
#endif

	// Copy only the region relevant to the RT
	D3D11_BOX srcBox;
	srcBox.left = 0;
	srcBox.right = m_Width;
	srcBox.top = 0;
	srcBox.bottom = m_Height;
	srcBox.front = m_StartW;
	srcBox.back = m_WSize;

	pContext->CopySubresourceRegion(m_pResource, D3D11CalcSubresource(m_MipSlice, m_StartArray, m_MipLevels), 0, 0, m_StartW, pRTD3D->GetD3D11Resource(), D3D11CalcSubresource(m_MipSlice, m_StartArray, m_MipLevels), &srcBox);
}

boolean CRTextureRTD3D11::DoLockReadOnlyPrepare()
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	// Create offscreen resource for CPU reading if it does not exist
	if(!m_pOffscreen)
	{
		switch(m_ResType)
		{
		case D3D11_RTV_DIMENSION_TEXTURE2D:
		case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.ArraySize = m_ArraySize;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
				desc.Format = m_Format;
				desc.Height = m_Height;
				desc.Width = m_Width;
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_STAGING;
				m_pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenTexture2D(desc);
				break;
			}

		case D3D11_RTV_DIMENSION_TEXTURE3D:
			{
				D3D11_TEXTURE3D_DESC desc;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
				desc.Format = m_Format;
				desc.Height = m_Height;
				desc.Width = m_Width;
				desc.Depth = m_WSize;
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.Usage = D3D11_USAGE_STAGING;
				m_pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenTexture3D(desc);
				break;
			}

		default: _DEBUG_ASSERT(FALSE); break;
		}
	}

	// Initiate readback from GPU
	D3D11_BOX srcBox;
	srcBox.left = 0;
	srcBox.right = m_Width;
	srcBox.top = 0;
	srcBox.bottom = m_Height;
	srcBox.front = m_StartW;
	srcBox.back = m_WSize;

	pContext->CopySubresourceRegion(m_pOffscreen, D3D11CalcSubresource(0, 0, 0), 0, 0, 0, m_pResource, D3D11CalcSubresource(m_MipSlice, m_StartArray, m_MipLevels), &srcBox);
}

byte* CRTextureRTD3D11::DoLockReadOnly(uint& pitch)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	// TODO: This only accesses 2D subresources
	_DEBUG_ASSERT(m_pOffscreen);
	D3D11_MAPPED_SUBRESOURCE subRes;
	HRESULT hr = pContext->Map(m_pOffscreen, 0, D3D11_MAP_READ, 0, &subRes);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	pitch = subRes.RowPitch;

	return (byte*) subRes.pData;
}

boolean CRTextureRTD3D11::DoUnlockReadOnly()
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	pContext->Unmap(m_pResource, 0);
}

void CRTextureRTD3D11::SetRTV(ID3D11Resource* pRes, ID3D11RenderTargetView* pRTV)
{
	// TODO:
	// Grab everything from desc and rtv desc
	m_MipSlice = 0;
	m_MipLevels = 1;
	m_StartArray = 0;
	m_ArraySize = 1;

	m_Depth = 1;
	m_StartW = 0;
	m_WSize = 1;

	m_pResource = pRes;
	m_pResource->GetType(&m_ResType);

	pRTV->GetDesc(&m_Desc);
	switch(m_Desc.ViewDimension)
	{
	case D3D11_RTV_DIMENSION_TEXTURE2D:
		m_MipSlice = m_Desc.Texture2D.MipSlice;
		break;

	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
		m_MipSlice = m_Desc.Texture2DArray.MipSlice;
		m_StartArray = m_Desc.Texture2DArray.FirstArraySlice;
		m_ArraySize = m_Desc.Texture2DArray.ArraySize;
		break;

	case D3D11_RTV_DIMENSION_TEXTURE3D:
		m_MipSlice = m_Desc.Texture3D.MipSlice;
		m_StartW = m_Desc.Texture3D.FirstWSlice; 
		m_WSize = m_Desc.Texture3D.WSize;
		break;

	default: _DEBUG_ASSERT(FALSE); break;
	}

	switch(m_ResType)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			D3D11_TEXTURE2D_DESC desc;
			((ID3D11Texture2D*)pRes)->GetDesc(&desc);
			m_MipLevels = desc.MipLevels;
			m_Width = desc.Width >> m_MipSlice;
			if(m_Width <= 0) m_Width = 1;
			m_Height = desc.Height >> m_MipSlice;
			if(m_Height <= 0) m_Height = 1;

			m_Format = desc.Format;
			break;
		}
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			D3D11_TEXTURE3D_DESC desc;
			((ID3D11Texture3D*)pRes)->GetDesc(&desc);
			m_MipLevels = desc.MipLevels;

			m_Width = desc.Width >> m_MipSlice;
			if(m_Width <= 0) m_Width = 1;
			m_Height = desc.Height >> m_MipSlice;
			if(m_Height <= 0) m_Height = 1;
			m_Depth = desc.Depth >> m_MipSlice;
			if(m_Depth <= 0) m_Depth = 1;

			m_Format = desc.Format;
			break;
		}

	default: _DEBUG_ASSERT(FALSE); break;
	}

	m_pRTView = pRTV;
}

ID3D11RenderTargetView* CRTextureRTD3D11::GetRTV()
{
	return m_pRTView;
}


_NAMESPACE_END
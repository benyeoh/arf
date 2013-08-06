//==============================================================================
//
//		CRResourceMgrD3D11.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Friday, April 07, 2013
//
//==============================================================================

#include "RendererD3D11Framework.h"

ID3D11Texture2D* CRResourceMgrD3D11::CreateD3DTexture(uint width, uint height, uint mipMapLevel, DXGI_FORMAT format, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag)
{
	ID3D11Device* pDevice = ((CRRendererD3D11*)m_pRenderer)->GetDevice();

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipMapLevel;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = usage;
	desc.BindFlags = bindFlag;

	if(usage & D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		desc.CPUAccessFlags = 0;

	desc.MiscFlags = 0;

	ID3D11Texture2D* pTex;
	pDevice->CreateTexture2D(&desc, NULL, &pTex);
	return pTex;
}

CRTextureRTD3D11* CRResourceMgrD3D11::CreateRenderTargetFromResource(ID3D11Resource* pRes, D3D11_RENDER_TARGET_VIEW_DESC* pDesc, uint width, uint height)
{
	ID3D11RenderTargetView* pRTV;
	hr = pDevice->CreateRenderTargetView(pRes, pDesc, &pRTV);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	CRTextureRTD3D11* pRT = _NEW CRTextureRTD3D11(m_pRenderer);
	pRT->SetRTV(pRTV, width, height);
	pRTV->Release();

	return pRT;
}

CRTexture2D* CRResourceMgrD3D11::DoCreateTexture2D(uint width, uint height, uint mipMapLevel, eRTexFormat format, eRTexUsage usage)
{
	CRTexture2DD3D11* pD3DTex = _NEW CRTexture2DD3D11(m_pRenderer);
	ID3D11Texture2D* pTex2D = CreateD3DTexture(width, height, mipMapLevel, TexFormatToD3D11Full(format), TexUsageToD3D11(usage), D3D11_BIND_SHADER_RESOURCE);
	pD3DTex->SetD3DTexture(pTex2D, NULL);
	pTex2D->Release();

	return pD3DTex;
}
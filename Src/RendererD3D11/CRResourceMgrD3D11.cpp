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

ID3D11Buffer* CRResourceMgrD3D11::CreateD3DVertexBuffer(uint bufferLen, D3D11_USAGE usage)
{
	ID3D11Device* pDevice = ((CRRendererD3D11*)m_pRenderer)->GetDevice();

	D3D11_BUFFER_DESC desc;
	
	if(usage & D3D11_USAGE_STAGING)
		desc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	if(usage & D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	else
		desc.CPUAccessFlags		= 0;

	desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth				= bufferLen;
	desc.MiscFlags				= 0;
	desc.Usage					= usage;
	desc.StructureByteStride	= 0;

	ID3D11Buffer* pBuffer;
	HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &pBuffer);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	pBuffer->AddRef();

	return pBuffer;
}

ID3D11Buffer* CRResourceMgrD3D11::CreateD3DIndexBuffer(uint bufferLen, D3D11_USAGE usage)
{
	ID3D11Device* pDevice = ((CRRendererD3D11*)m_pRenderer)->GetDevice();

	D3D11_BUFFER_DESC desc;

	if(usage & D3D11_USAGE_STAGING)
		desc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	if(usage & D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	else
		desc.CPUAccessFlags		= 0;

	desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth				= bufferLen;
	desc.MiscFlags				= 0;
	desc.Usage					= usage;
	desc.StructureByteStride	= 0;

	ID3D11Buffer* pBuffer;
	HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &pBuffer);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	pBuffer->AddRef();

	return pBuffer;
}

ID3D11Texture2D* CRResourceMgrD3D11::CreateD3DTexture(uint width, uint height, uint mipMapLevel, uint arraySize,
													  DXGI_FORMAT format, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag, D3D11_RESOURCE_MISC_FLAG miscFlags)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipMapLevel;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = usage;
	desc.BindFlags = bindFlag;

	if(usage & D3D11_USAGE_STAGING)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	if(usage & D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		desc.CPUAccessFlags = 0;

	desc.MiscFlags = miscFlags;

	return CreateD3DTexture(desc);
}

ID3D11Texture2D* CRResourceMgrD3D11::CreateD3DTexture(D3D11_TEXTURE2D_DESC& desc)
{
	ID3D11Device* pDevice = ((CRRendererD3D11*)m_pRenderer)->GetDevice();
	ID3D11Texture2D* pTex;
	pDevice->CreateTexture2D(&desc, NULL, &pTex);
	return pTex;
}

ID3D11Texture2D* CRResourceMgrD3D11::CreateD3DTexture3D(D3D11_TEXTURE3D_DESC& desc)
{
	ID3D11Device* pDevice = ((CRRendererD3D11*)m_pRenderer)->GetDevice();
	ID3D11Texture3D* pTex;
	pDevice->CreateTexture3D(&desc, NULL, &pTex);
	return pTex;
}

CRTextureRTD3D11* CRResourceMgrD3D11::CreateRenderTargetFromResource(ID3D11Resource* pRes, D3D11_RENDER_TARGET_VIEW_DESC* pDesc)
{
	ID3D11RenderTargetView* pRTV;
	hr = pDevice->CreateRenderTargetView(pRes, pDesc, &pRTV);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	CRTextureRTD3D11* pRT = _NEW CRTextureRTD3D11(m_pRenderer);
	pRT->SetRTV(pRes, pRTV);
	pRTV->Release();

	return pRT;
}

CRTextureCube* CRResourceMgrD3D11::DoCreateTextureCube(uint size, uint mipMapLevel, eRTexFormat format, eRTexUsage usage)
{
	// TODO: Create without typeless formats for now
	CRTextureCubeD3D11* pD3DTex = _NEW CRTextureCubeD3D11(m_pRenderer);
	ID3D11Texture2D* pTex2D = CreateD3DTexture(width, height, mipMapLevel, 6, TexFormatToD3D11(format), TexUsageToD3D11(usage), D3D11_BIND_SHADER_RESOURCE, 0);
	pD3DTex->SetD3DTexture(pTex2D, NULL);
	pTex2D->Release();

	return pD3DTex;
}

CRTexture2D* CRResourceMgrD3D11::DoCreateTexture2D(uint width, uint height, uint mipMapLevel, eRTexFormat format, eRTexUsage usage)
{
	CRTexture2DD3D11* pD3DTex = _NEW CRTexture2DD3D11(m_pRenderer);
	ID3D11Texture2D* pTex2D = CreateD3DTexture(width, height, mipMapLevel, 1, TexFormatToD3D11(format), TexUsageToD3D11(usage), D3D11_BIND_SHADER_RESOURCE, 0);
	pD3DTex->SetD3DTexture(pTex2D, NULL);
	pTex2D->Release();

	return pD3DTex;
}

ID3D11Texture2D* CRResourceMgrD3D11::CreateOffscreenTexture2D(D3D11_TEXTURE2D_DESC& desc)
{
	ID3D11Texture2D* pTex = CreateD3DTexture(desc.Width, desc.Height, desc.MipLevels, desc.ArraySize, desc.Format, D3D11_USAGE_STAGING, 0, desc.MiscFlags);
	return pTex;
}

ID3D11Texture3D* CRResourceMgrD3D11::CreateOffscreenTexture3D(D3D11_TEXTURE3D_DESC& desc)
{
	D3D11_TEXTURE3D_DESC offScreenDesc = desc;
	offScreenDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	offScreenDesc.Usage = D3D11_USAGE_STAGING;
	offScreenDesc.BindFlags = 0;

	ID3D11Texture3D* pTex = CreateD3DTexture3D(desc);
	return pTex;
}


ID3D11Buffer* CRResourceMgrD3D11::CreateOffscreenVB(const D3D11_BUFFER_DESC& desc)
{
	ID3D11Buffer* pVB = CreateD3DVertexBuffer(desc.ByteWidth, D3D11_USAGE_STAGING);
	return pVB;
}

ID3D11Buffer* CRResourceMgrD3D11::CreateOffscreenIB(const D3D11_BUFFER_DESC& desc)
{
	ID3D11Buffer* pIB = CreateD3DIndexBuffer(desc.ByteWidth, D3D11_USAGE_STAGING);
	return pIB;
}


//================================================================================
//
//		CRRendererD3D11.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/5/2013
//
//================================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

boolean	CRRendererD3D11::DoInitialize(RDisplayInfo& info, RRendererInfo& rendererInfo)
{
	DXGI_SWAP_CHAIN_DESC scDesc;
	memset(&scDesc, 0, sizeof(scDesc));
	scDesc.BufferDesc.Width = info.backBufferWidth;
	scDesc.BufferDesc.Height = info.backBufferHeight;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferCount = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = (HWND) info.hWnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.Windowed = info.fullScreen ? TRUE : FALSE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	
	UINT flags = 0;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	// Swap chain creation
	D3D11CreateDeviceAndSwapChain(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL, 
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scDesc,
		&pSwapChain,
		&pDevice,
		NULL,
		&pDeviceContext);

	m_pSwapChain = pSwapChain;
	m_pDeviceContext = pDeviceContext;
	m_pDevice = pDevice;

	pSwapChain->Release();
	pDeviceContext->Release();
	pDevice->Release();

	ID3D11RenderTargetView* pRTView;
	ID3D11Texture2D* pTexBB;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pTexBB);
	pDevice->CreateRenderTargetView(pTexBB, NULL, &pRTView);
	m_pRTView = pRTView;
	
	pRTView->Release();
	pTexBB->Release();

	// Create depth stencil surface
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = info.backBufferWidth;
	descDepth.Height = info.backBufferHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	pDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
	m_pDepthTexture = pDepthStencil;
	
	// Set depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
	ID3D11DepthStencilView* pDepthView;
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.Flags = 0;
	depthViewDesc.Texture2D.MipSlice = 0;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	pDevice->CreateDepthStencilView(pDepthStencil, &depthViewDesc, &pDepthView);
	m_pDepthView = pDepthView;

	pDepthView->Release();
	pDepthStencil->Release();

	pDeviceContext->OMSetRenderTargets(1, &pRTView, pDepthView);

	return TRUE;
}

boolean CRRendererD3D11::DoSetDisplayInfo(RDisplayInfo& info)
{
	return TRUE;
}

boolean CRRendererD3D11::DoQueryDeviceReady()
{
	return TRUE;
}

boolean CRRendererD3D11::DoResetDevice()
{
	return TRUE;
}

void CRRendererD3D11::DoClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth)
{
	if(pColor)
	{
		ID3D11RenderTargetView* pRTView;
		pRTView = m_pRTView;
		m_pDeviceContext->ClearRenderTargetView(pRTView, pColor->getData());
	}

	if(pDepth)
	{
		ID3D11DepthStencilView* pDepthView;
		pDepthView = m_pDepthView;
		m_pDeviceContext->ClearDepthStencilView(pDepthView,  D3D11_CLEAR_DEPTH |  D3D11_CLEAR_STENCIL, *pDepth, 0);
	}
}

void CRRendererD3D11::DoEndRender()
{
}

void CRRendererD3D11::DoBeginRender()
{
}

void CRRendererD3D11::DoDisplayFrame()
{
	m_pSwapChain->Present(0, 0);
}

void CRRendererD3D11::DoRenderPrimitive(RRenderOp* pRenderOp)
{
}

ScratchPad* CRRendererD3D11::GetScratchPad(uint minSize)
{
	ScratchPad* pPad = m_ScratchPadPool.Get();
	if(pPad->size < minSize)
		pPad->Realloc(minSize);

	return pPad;
}

void CRRendererD3D11::DisposeScratchPad(ScratchPad* pPad)
{
	m_ScratchPadPool.Dispose(pPad);
}


_NAMESPACE_END
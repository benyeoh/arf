//================================================================================
//
//		CRRendererD3D11.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/5/2013
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CRRendererD3D11 : public CRRenderer
{
private:
	ID3D11DevicePtr m_pDevice;
	ID3D11DeviceContextPtr m_pDeviceContext;
	IDXGISwapChainPtr m_pSwapChain;
	
	ID3D11Texture2DPtr m_pDepthTexture;
	
	ID3D11RenderTargetViewPtr m_pRTView;
	ID3D11DepthStencilViewPtr m_pDepthView;

	// TODO: This should scale with the number of contexts
	ObjectPoolAtomic<ScratchPad, 4> m_ScratchPadPool;

public:
	CRRendererD3D11()
		: CRRenderer()
	{
	}

	~CRRendererD3D11()
	{
	}

public:
	boolean	DoInitialize(RDisplayInfo& info, RRendererInfo& rendererInfo);
	boolean DoSetDisplayInfo(RDisplayInfo& info);

	boolean DoQueryDeviceReady();
	boolean DoResetDevice();
	void DoClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth);
	void DoEndRender();
	void DoBeginRender();
	void DoDisplayFrame();

	void DoRenderPrimitive(RRenderOp* pRenderOp);

	void DoOnRestoreDevice() {}
	void DoOnResetDevice() {}
	void DoOnLostDevice() {}

	CRResourceMgrD3D11* GetResourceMgrD3D() { return (CRResourceMgrD3D11*) m_pResourceMgr; }
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetImmediateContext() { return m_pDeviceContext; }
	
	ScratchPad* GetScratchPad(uint minSize);
	void DisposeScratchPad(ScratchPad* pPad);

	// TODO: For now
	ID3D11DeviceContext* GetCurrentContext() { return m_pDeviceContext; }
};

_NAMESPACE_END
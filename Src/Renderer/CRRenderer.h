//==============================================================================
//
//		CRRenderer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRRenderer : public CRefCounted<IRRenderer>
{
private:
	boolean	m_IsDeviceReady;
	boolean m_ForceDeviceReset;
	
protected:
	//CRPipeline*		m_pPipeline;
	CRResourceMgr*		m_pResourceMgr;
	IAppCallback*		m_pApplication;
	RDisplayInfo		m_PresentInfo;
	RRendererInfo		m_RendererInfo;
	RMetrics			m_Metrics;
	IRTextureRTPtr		m_pBackBufferColor;
	IRDepthBufferPtr	m_pBackBufferDepth;

public:
	CRRenderer()
		: //m_pPipeline(NULL)
		  m_pResourceMgr(NULL)
		, m_pApplication(NULL)
		, m_IsDeviceReady(TRUE)
		, m_ForceDeviceReset(FALSE)
	{
		// Set the (internal) global pointer for the renderer
		//g_pRenderer = this;
	}

	virtual ~CRRenderer()
	{
		m_pBackBufferDepth = NULL;
		m_pBackBufferColor = NULL;

		//_DELETE(m_pPipeline);
		_DELETE(m_pResourceMgr);
		// Do not touch IAppCallback
	}

private:
	void OnLostDevice();	
	void OnResetDevice();
	void OnRestoreDevice();
	void ResetMetrics();
	
protected:	
	_PURE( boolean DoInitialize(RDisplayInfo& info, RRendererInfo& rendererInfo) )
	_PURE( boolean DoSetDisplayInfo(RDisplayInfo& info) )
	_PURE( boolean DoQueryDeviceReady() )
	_PURE( boolean DoResetDevice() )
	_PURE( void DoClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth) )
	_PURE( void DoEndRender() )
	_PURE( void DoBeginRender() )
	_PURE( void DoDisplayFrame() )
	_PURE( void DoRenderPrimitive(RRenderOp* pOp) )
	//_PURE( void DoGetBackBuffer(IRTextureRT* pRT) )
	_PURE( void DoOnRestoreDevice() )
	_PURE( void DoOnResetDevice() )
	_PURE( void DoOnLostDevice() )

public:
	//------------------------------------------------------------------------------
	//	Initialize()
	//		
	//------------------------------------------------------------------------------
	boolean	Initialize(const RInitInfo& info);

	boolean SetDisplayInfo(const RDisplayInfo& info);
	const RDisplayInfo& GetDisplayInfo();

	const RRendererInfo& GetRendererInfo();
	
	boolean BeginRender();
	void EndRender();
	void ClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth);
	//void GetBackBuffer(IRTextureRT* pRT);

	IRTextureRT* GetBackBufferColor();
	IRDepthBuffer* GetBackBufferDepth();

	void DisplayFrame();
	
	void RenderPrimitive(RRenderOp* pRenderOp);
	
	RMetrics& GetMetricsInternal() { return m_Metrics; }
	const RMetrics& GetMetrics() { return m_Metrics; }
	
	//------------------------------------------------------------------------------
	//	GetRPipeline()
	//		
	//------------------------------------------------------------------------------	
	//IRPipeline& GetRPipeline() { return *m_pPipeline; }
	//CRPipeline& GetCRPipeline() { return *m_pPipeline; }
	
	//------------------------------------------------------------------------------
	//	GetRResourceMgr()
	//		
	//------------------------------------------------------------------------------
	IRResourceMgr& GetRResourceMgr() { return *m_pResourceMgr; }
	CRResourceMgr& GetCRResourceMgr() { return *m_pResourceMgr; }
	
	//------------------------------------------------------------------------------
	//	IAppCallback()
	//		
	//------------------------------------------------------------------------------
	IAppCallback& GetAppCallback() { return *m_pApplication; }
};

_NAMESPACE_END
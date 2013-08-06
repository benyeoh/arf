//==============================================================================
//
//		CRRenderer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

void 
CRRenderer::ResetMetrics()
{
	m_Metrics.numDrawCalls = 0;
	m_Metrics.numPrimitivesRendered = 0;
	m_Metrics.numVerticesRendered = 0;
	m_Metrics.numBeginPass = 0;
	m_Metrics.numBeginTechnique = 0;
	m_Metrics.numRenderGroups = 0;
	m_Metrics.numApplyConstantParam = 0;
	m_Metrics.numApplyDynamicParam = 0;	
}

boolean
CRRenderer::Initialize(const RInitInfo& rInfo)
{	
	if(!m_pApplication)
	{		
		m_pApplication = rInfo.pApplication;		
		_DEBUG_ASSERT(m_pApplication);
		m_PresentInfo = rInfo.presentInfo;

		boolean result = DoInitialize(m_PresentInfo, m_RendererInfo);
		if(result)
		{
			m_pBackBufferColor	= m_pResourceMgr->CreateBackBufferColor();
			m_pBackBufferDepth	= m_pResourceMgr->CreateBackBufferDepth();
			return result;
		}
	}
	
	return FALSE;
}

boolean 
CRRenderer::SetDisplayInfo(const RDisplayInfo& info)
{
	m_PresentInfo = info;
	if(DoSetDisplayInfo(m_PresentInfo))
	{
		m_ForceDeviceReset = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

const RDisplayInfo& 
CRRenderer::GetDisplayInfo()
{
	return m_PresentInfo;
}
	
void
CRRenderer::RenderPrimitive(RRenderOp* pRenderOp)
{
	DoRenderPrimitive(pRenderOp);
	_INC_METRIC(this, numDrawCalls);
	_INC_METRIC_NUM(this, numPrimitivesRendered, 
		(pRenderOp->pIB->GetNumIndices() / (pRenderOp->prim == PRIM_TRIANGLE ? 3 : 2)) * gmtl::Math::Max((uint)1, pRenderOp->numInstances));
	_INC_METRIC_NUM(this, numVerticesRendered, 
		pRenderOp->pVBGroup->GetVertexBuffer(0)->GetNumVertices() * gmtl::Math::Max((uint)1, pRenderOp->numInstances));
}

boolean 
CRRenderer::BeginRender()
{
	if(DoQueryDeviceReady() && !m_ForceDeviceReset)
	{
		// If the device was previously lost
		// make sure to reset the device
		if(!m_IsDeviceReady)
		{
			OnRestoreDevice();
			
			if(!DoResetDevice())
				return FALSE;				
			
			OnResetDevice();
		}
	
		DoBeginRender();		
		m_IsDeviceReady = TRUE;
		
		ResetMetrics();
	}
	else
	if(m_IsDeviceReady)
	{
		OnLostDevice();	
		m_IsDeviceReady = FALSE;
		m_ForceDeviceReset = FALSE;
	}
	
	return m_IsDeviceReady;
}

void 
CRRenderer::EndRender()
{
	DoEndRender();
}

void 
CRRenderer::DisplayFrame()
{
	if(m_IsDeviceReady)
		DoDisplayFrame();
}

//void 
//CRRenderer::GetBackBuffer(IRTextureRT* pRT)
//{
//	DoGetBackBuffer(pRT);
//}

void
CRRenderer::ClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth)
{
	DoClearBackBuffer(pColor, pDepth);
}

IRTextureRT* CRRenderer::GetBackBufferColor()
{
	return m_pBackBufferColor;
}

IRDepthBuffer* CRRenderer::GetBackBufferDepth()
{
	return m_pBackBufferDepth;
}

void 
CRRenderer::OnLostDevice()
{
	//m_pPipeline->OnLostDevice();
	m_pResourceMgr->OnLostDevice();
	
	DoOnLostDevice();
}
	
void 
CRRenderer::OnResetDevice()
{
	//m_pPipeline->OnResetDevice();
	m_pResourceMgr->OnResetDevice();

	DoOnResetDevice();
}

void 
CRRenderer::OnRestoreDevice()
{
	//m_pPipeline->OnRestoreDevice();
	m_pResourceMgr->OnRestoreDevice();

	DoOnRestoreDevice();
}

const RRendererInfo& 
CRRenderer::GetRendererInfo()
{
	return m_RendererInfo;
}

_NAMESPACE_END
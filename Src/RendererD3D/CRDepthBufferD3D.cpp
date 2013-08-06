//==============================================================================
//
//		CRDepthBufferD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/25/2007
//
//==============================================================================

#include "RendererD3D.h"

_NAMESPACE_BEGIN

void 
CRDepthBufferD3D::SetDepthStencilSurface(IDirect3DSurface9* pDepthStencil, boolean isBackBufferDepth)
{
	m_pDepthStencil = pDepthStencil;
	m_IsBackBufferDepth = isBackBufferDepth;
}

IDirect3DSurface9* 
CRDepthBufferD3D::GetDepthStencilSurface()
{
	return m_pDepthStencil;
}

void 
CRDepthBufferD3D::OnResetDevice()
{
	if(m_IsBackBufferDepth)
	{
		_DEBUG_TRACE(_W("Resetting Depth Buffer: %s\n"), GetResourceName());
		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetBackBufferDepth(this);
		_DEBUG_ASSERT(success);
	}
	else
	{
		_DEBUG_TRACE(_W("Resetting Depth Buffer: %s\n"), GetResourceName());
		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetDepthBuffer(this);
		_DEBUG_ASSERT(success);
	}
}

void 
CRDepthBufferD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost Depth Buffer: %s\n"), GetResourceName());

	m_pDepthStencil = NULL;
}

void 
CRDepthBufferD3D::OnRestoreDevice()
{
}

_NAMESPACE_END